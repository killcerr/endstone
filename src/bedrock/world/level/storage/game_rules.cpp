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

#include "bedrock/world/level/storage/game_rules.h"

#include "bedrock/network/packet/game_rules_changed_packet.h"

bool GameRule::_set(const Value &value, bool *validated, ValidationError *error_output)
{
    auto set_validated = [validated](bool value) {
        if (validated != nullptr) {
            *validated = value;
        }
    };

    if (validate_value_callback_ && !validate_value_callback_(value, error_output)) {
        set_validated(false);
        return false;
    }

    bool value_unchanged = false;
    switch (type_) {
    case Type::Bool:
        if (value.index() == value_.index()) {
            value_unchanged = std::get<bool>(value) == std::get<bool>(value_);
        }
        break;
    case Type::Int:
        if (value.index() == value_.index()) {
            value_unchanged = std::get<int>(value) == std::get<int>(value_);
        }
        break;
    case Type::Float:
        if (value.index() == value_.index()) {
            value_unchanged = std::get<float>(value) == std::get<float>(value_);
        }
        break;
    case Type::Invalid:
    default:
        break;
    }

    if (value_unchanged) {
        set_validated(true);
        return false;
    }

    value_ = value;
    is_default_set_ = false;
    set_validated(true);
    return true;
}

std::shared_ptr<GameRulesChangedPacket> GameRules::setGameRule(GameRuleId rule, GameRule::Value value,
                                                               bool return_packet, bool *value_validated,
                                                               bool *value_changed,
                                                               GameRule::ValidationError *error_output)
{
    if (rule < 0 || rule >= game_rules_.size()) {
        if (value_validated != nullptr) {
            *value_validated = false;
        }
        if (value_changed != nullptr) {
            *value_changed = false;
        }
        return nullptr;
    }

    auto &game_rule = game_rules_[rule];
    return _setGameRule(&game_rule, value, game_rule.type_, return_packet, value_validated, value_changed,
                        error_output);
}

std::shared_ptr<GameRulesChangedPacket> GameRules::_setGameRule(GameRule *game_rule, GameRule::Value value,
                                                                GameRule::Type type, bool return_packet,
                                                                bool *value_validated, bool *value_changed,
                                                                GameRule::ValidationError *error_output)
{
    if (value_validated != nullptr) {
        *value_validated = false;
    }
    if (value_changed != nullptr) {
        *value_changed = false;
    }

    if (game_rule == nullptr) {
        return nullptr;
    }

    switch (type) {
    case GameRule::Type::Bool:
        value = std::get<bool>(value);
        break;
    case GameRule::Type::Int:
        value = std::get<int>(value);
        break;
    case GameRule::Type::Float:
        value = std::get<float>(value);
        break;
    case GameRule::Type::Invalid:
    default:
        return nullptr;
    }

    bool validated = false;
    const bool changed = game_rule->_set(value, &validated, error_output);

    if (value_validated != nullptr) {
        *value_validated = validated;
    }
    if (value_changed != nullptr) {
        *value_changed = changed;
    }
    if (!changed) {
        return nullptr;
    }

    // todo(gamerule): publish the game rule change.
    if (!return_packet) {
        return nullptr;
    }

    const auto packet = MinecraftPackets::createPacket(MinecraftPacketIds::GameRulesChanged);
    const auto pk = std::static_pointer_cast<GameRulesChangedPacket>(packet);
    pk->payload.rule_data.rules.push_back(*game_rule);
    return pk;
}
