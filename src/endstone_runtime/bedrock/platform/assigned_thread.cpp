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

#include "bedrock/platform/threading/assigned_thread.h"

#include "endstone/detail/hook.h"

namespace Bedrock::Threading {

bool AssignedThread::isOnThread() const
{
    return std::this_thread::get_id() == assigned_id_;
}

AssignedThread &getServerThread()
{
    return ENDSTONE_HOOK_CALL_ORIGINAL(&getServerThread);
}

}  // namespace Bedrock::Threading