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

#include "endstone/core/spdlog/thread_formatter.h"

#include <spdlog/details/fmt_helper.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace endstone::core {

namespace {
std::string getThreadName()
{
#ifdef _WIN32
    using GetThreadDescriptionFn = HRESULT(WINAPI *)(HANDLE, PWSTR *);
    static const auto get_thread_description = [] {
        auto *const kernel32 = GetModuleHandleA("Kernel32.dll");
        return kernel32 == nullptr
                 ? nullptr
                 : reinterpret_cast<GetThreadDescriptionFn>(GetProcAddress(kernel32, "GetThreadDescription"));
    }();
    if (get_thread_description == nullptr) {
        return {};
    }

    PWSTR description = nullptr;
    if (FAILED(get_thread_description(GetCurrentThread(), &description)) || description == nullptr) {
        return {};
    }
    const auto size = WideCharToMultiByte(CP_UTF8, 0, description, -1, nullptr, 0, nullptr, nullptr);
    std::string name(size > 0 ? size - 1 : 0, '\0');
    if (size > 0) {
        WideCharToMultiByte(CP_UTF8, 0, description, -1, name.data(), size, nullptr, nullptr);
    }
    LocalFree(description);
    return name;
#else
    char buffer[64];
    if (pthread_getname_np(pthread_self(), buffer, sizeof(buffer)) != 0) {
        return {};
    }
    return buffer;
#endif
}
}  // namespace

void ThreadFormatter::format(const spdlog::details::log_msg &msg, const std::tm &, spdlog::memory_buf_t &dest)
{
    if (const auto name = getThreadName(); !name.empty()) {
        if (name == "MC_SERVER") {
            spdlog::details::fmt_helper::append_string_view("Server", dest);
        }
        else {
            spdlog::details::fmt_helper::append_string_view(name, dest);
        }
        return;
    }
    spdlog::details::fmt_helper::append_int(msg.thread_id, dest);
}

std::unique_ptr<spdlog::custom_flag_formatter> ThreadFormatter::clone() const
{
    return spdlog::details::make_unique<ThreadFormatter>();
}

}  // namespace endstone::core
