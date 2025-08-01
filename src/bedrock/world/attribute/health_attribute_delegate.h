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

#pragma once

#include "bedrock/world/attribute/attribute_instance.h"
#include "bedrock/world/attribute/attribute_instance_delegate.h"

class Mob;
class HealthAttributeDelegate : public AttributeInstanceDelegate {
public:
    HealthAttributeDelegate(const AttributeInstance &, Mob *);
    ENDSTONE_HOOK float change(float old_value, float new_value, const AttributeBuff &buff) override;

private:
    std::int32_t tick_counter_;
    Mob *mob_;
};
BEDROCK_STATIC_ASSERT_SIZE(HealthAttributeDelegate, 32, 24);
