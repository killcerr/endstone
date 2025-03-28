// Copyright (c) 2023, The Endstone Project. (https://endstone.dev) All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "endstone/core/plugin/cpp_plugin_loader.h"

#include <filesystem>
#include <regex>
namespace fs = std::filesystem;

#include "endstone/core/logger_factory.h"
#include "endstone/core/server.h"
#include "endstone/plugin/plugin.h"

#ifdef _WIN32
#include <Windows.h>
#define LOAD_LIBRARY(file)             LoadLibraryA(file)
#define GET_FUNCTION(module, function) GetProcAddress(module, function)
#define GET_ERROR()                    GetLastError()
#define CLOSE_LIBRARY(module)          FreeLibrary(module)
#elif __linux__
#include <dlfcn.h>
#define LOAD_LIBRARY(file)             dlopen(file, RTLD_NOW)
#define GET_FUNCTION(module, function) dlsym(module, function)
#define GET_ERROR()                    dlerror()
#define CLOSE_LIBRARY(module)          dlclose(module)
#endif

namespace endstone::core {

Plugin *CppPluginLoader::loadPlugin(std::string file)
{
    auto &logger = server_.getLogger();
    auto path = fs::path(file);
    if (!exists(path)) {
        logger.error("Could not load plugin from '{}': Provided file does not exist.", path.string());
        return nullptr;
    }

    auto *module = LOAD_LIBRARY(file.c_str());
    if (!module) {
        logger.error("Failed to load c++ plugin from {}: LoadLibrary failed with code {}.", file, GET_ERROR());
        return nullptr;
    }

    using InitPlugin = Plugin *(*)();
    auto init_plugin = GET_FUNCTION(module, "init_endstone_plugin");
    if (!init_plugin) {
        CLOSE_LIBRARY(module);
        logger.error("Failed to load c++ plugin from {}: No entry point. Did you forget ENDSTONE_PLUGIN?", file);
        return nullptr;
    }

    auto *plugin = reinterpret_cast<InitPlugin>(init_plugin)();
    if (!plugin) {
        CLOSE_LIBRARY(module);
        logger.error("Failed to load c++ plugin from {}: Invalid plugin instance.", file);
        return nullptr;
    }

    static const std::string supported_api_version = ENDSTONE_API_VERSION;
    if (plugin->getDescription().getAPIVersion() != supported_api_version) {
        logger.error("Error occurred when trying to load plugin '{}': plugin was compiled for Endstone "
                     "API version: {}, but the server has an incompatible API version: {}.",
                     plugin->getDescription().getName(), plugin->getDescription().getAPIVersion(),
                     supported_api_version);
        return nullptr;
    }

    return plugins_.emplace_back(plugin).get();
}

std::vector<std::string> CppPluginLoader::getPluginFileFilters() const
{
#ifdef _WIN32
    return {"\\.dll$"};
#elif __linux__
    return {"\\.so$"};
#endif
}

}  // namespace endstone::core

#undef LOAD_LIBRARY
#undef GET_FUNCTION
#undef GET_ERROR
#undef CLOSE_LIBRARY
