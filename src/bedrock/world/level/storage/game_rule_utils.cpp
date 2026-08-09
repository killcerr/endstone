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

#include "bedrock/world/level/storage/game_rule_utils.h"

#include "bedrock/network/packet/game_rules_changed_packet.h"
#include "bedrock/world/level/level_interface.h"

namespace GameRuleUtils {
template <typename T>
void _setGameRule(ILevel &level, const GameRuleId &rule_id, const T &previous_value, const T &value,
                  GameRule::ValidationError *validation_error, bool send_telemetry)
{
    auto &game_rules = level.getGameRules();
    // Endstone: shared_ptr, as GameRules hands back the createPacket factory's pointer.
    const auto packet = game_rules.setRule(rule_id, value, true, nullptr, nullptr, validation_error);
    if (!packet) {
        return;
    }
    level.getPacketSender()->sendBroadcast(*packet);
    if (send_telemetry) {
        // todo(gamerule): record the change telemetry with previous_value, value and the rule name.
    }
}

void setGameRule(ILevel &level, const GameRuleId &rule_id, std::variant<int, float, bool> value,
                 GameRule::ValidationError *validation_error, bool send_telemetry)
{
    const auto *rule = level.getGameRules().getRule(rule_id);
    switch (value.index()) {
    case 0:
        _setGameRule<int>(level, rule_id, rule != nullptr ? std::get<int>(rule->getValue()) : 0, std::get<int>(value),
                          validation_error, send_telemetry);
        break;
    case 1:
        _setGameRule<float>(level, rule_id, rule != nullptr ? std::get<float>(rule->getValue()) : 0.0F,
                            std::get<float>(value), validation_error, send_telemetry);
        break;
    case 2:
        _setGameRule<bool>(level, rule_id, rule != nullptr && std::get<bool>(rule->getValue()), std::get<bool>(value),
                           validation_error, send_telemetry);
        break;
    default:
        break;
    }
}
}  // namespace GameRuleUtils
