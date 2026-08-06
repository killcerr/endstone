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

#include "bedrock/core/debug/debug_utils.h"

#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace DebugUtils {

#ifdef _WIN32
void setThreadName(const std::string &name)
{
    using SetThreadDescriptionFn = HRESULT(WINAPI *)(HANDLE, PCWSTR);

    auto *const kernel32 = GetModuleHandleA("Kernel32.dll");
    if (kernel32 == nullptr) {
        return;
    }

    const auto set_thread_description =
        reinterpret_cast<SetThreadDescriptionFn>(GetProcAddress(kernel32, "SetThreadDescription"));
    if (set_thread_description == nullptr) {
        return;
    }

    std::wstring wide(name.size(), L' ');
    std::copy(name.begin(), name.end(), wide.begin());
    set_thread_description(GetCurrentThread(), wide.c_str());
}
#else
namespace {

void _setThreadName(const std::string &name)
{
    pthread_setname_np(pthread_self(), name.c_str());
}

}  // namespace

void setThreadName(const std::string &name)
{
    _setThreadName(name);
}
#endif

}  // namespace DebugUtils
