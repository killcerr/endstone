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

#include <vector>

#include "bedrock/network/packet.h"
#include "bedrock/world/level/storage/game_rules.h"

class GameRulesChangedPacketData {
public:
    std::vector<GameRule> rules;
};

struct GameRulesChangedPacketPayload {
    GameRulesChangedPacketData rule_data;
};

class GameRulesChangedPacket : public Packet {
public:
    GameRulesChangedPacketPayload payload;
    SerializationMode serialization_mode{SerializationMode::CerealOnly};
};
BEDROCK_STATIC_ASSERT_SIZE(GameRulesChangedPacket, 80, 80);
