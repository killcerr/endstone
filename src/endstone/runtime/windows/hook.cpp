// Copyright (c) 2024, The Endstone Project. (https://endstone.dev) All Rights Reserved.
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

#ifdef _WIN32

#include <Windows.h>

// DbgHelp.h must be included after Windows.h
#include <DbgHelp.h>

#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>
#include <unordered_map>

#include <spdlog/spdlog.h>
#include <toml++/toml.h>

#include "endstone/core/platform.h"
#include "endstone/runtime/hook.h"

namespace endstone::hook {

enum SymTagEnum {
    SymTagPublicSymbol = 10,
};

namespace {
void enumerate_symbols(const char *path, std::function<bool(const std::string &, std::size_t, std::uint32_t)> callback,
                       bool load_symbol)
{
    auto start = std::chrono::high_resolution_clock::now();

    HANDLE current_process = GetCurrentProcess();
    HANDLE handle;
    if (!DuplicateHandle(current_process, current_process, current_process, &handle, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
        throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "DuplicateHandle failed");
    }

    if (!SymInitialize(handle, nullptr, FALSE)) {
        throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "SymInitialize failed");
    }

    SymSetOptions(NULL);  // do not undecorate symbols by default

    auto module_base =
        SymLoadModuleEx(handle, nullptr, path, nullptr, 0, 0, nullptr, load_symbol ? 0 : SLMFLAG_NO_SYMBOLS);
    if (!module_base) {
        throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "SymLoadModuleEx failed");
    }

    struct UserContext {
        std::size_t module_base;
        std::function<bool(const std::string &, std::size_t, std::uint32_t)> callback;
    };
    auto user_context = UserContext{module_base, std::move(callback)};

    SymSearch(
        handle, module_base, 0, SymTagPublicSymbol, "*", 0,
        [](PSYMBOL_INFO info /*pSymInfo*/, ULONG /*SymbolSize*/, PVOID user_context /*UserContext*/) -> BOOL {
            auto *ctx = static_cast<UserContext *>(user_context);
            if (!(ctx->callback)(info->Name, info->Address - ctx->module_base, info->Flags)) {
                return FALSE;
            }
            return TRUE;
        },
        &user_context, SYMSEARCH_GLOBALSONLY);

    SymCleanup(handle);

    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    spdlog::debug("enumerate_symbols(): time elapsed: {}ms", duration.count());
}
}  // namespace

const std::unordered_map<std::string, void *> &get_detours()
{
    static std::unordered_map<std::string, void *> detours;
    if (!detours.empty()) {
        return detours;
    }

    auto *module_base = core::get_module_base();
    const auto module_pathname = core::get_module_pathname();
    const auto &targets = get_targets();

    enumerate_symbols(  //
        module_pathname.c_str(),
        [&](const std::string &name, std::size_t offset, std::uint32_t flags) -> bool {
            if (flags & SYMFLAG_EXPORT) {
                spdlog::debug("D: {} -> 0x{:x}", name, offset);
                auto *detour = static_cast<char *>(module_base) + offset;
                detours.emplace(name, detour);
            }
            return true;
        },
        false);  // set load_symbol to false so symbols are limited to the export table
    return detours;
}

const std::unordered_map<std::string, void *> &get_targets()
{
    static std::unordered_map<std::string, void *> targets;
    if (!targets.empty()) {
        return targets;
    }

    auto *executable_base = core::get_executable_base();
    const auto module_pathname = core::get_module_pathname();
    auto symbol_path = std::filesystem::path{module_pathname}.parent_path() / "symbols.toml";
    auto tbl = toml::parse_file(symbol_path.string());
    tbl["windows"].as_table()->for_each([executable_base](const toml::key &key, auto &&val) {
        if constexpr (toml::is_integer<decltype(val)>) {
            auto offset = val.get();
            spdlog::debug("T: {} -> 0x{:x}", key.data(), offset);
            auto *target = static_cast<char *>(executable_base) + offset;
            targets.emplace(key.data(), target);
        }
    });

    return targets;
}

}  // namespace endstone::hook

#endif
