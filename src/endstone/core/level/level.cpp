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

#include "endstone/core/level/level.h"

#include <cstdlib>
#include <ranges>
#include <stdexcept>
#include <utility>

#include <boost/algorithm/string.hpp>

#include "bedrock/entity/gamerefs_entity/gamerefs_entity.h"
#include "bedrock/network/packet/game_rules_changed_packet.h"
#include "bedrock/network/packet_sender.h"
#include "bedrock/world/level/dimension/dimension.h"
#include "bedrock/world/level/dimension/vanilla_dimensions.h"
#include "bedrock/world/level/dimension_manager.h"
#include "bedrock/world/level/level.h"
#include "bedrock/world/level/storage/game_rules.h"
#include "endstone/core/actor/actor.h"
#include "endstone/core/level/dimension.h"
#include "endstone/level/dimension.h"

namespace endstone::core {

EndstoneLevel::EndstoneLevel(::Level &level) : server_(EndstoneServer::getInstance()), level_(level)
{
    // Load all vanilla dimensions on start up
    level.getOrCreateDimension(VanillaDimensions::Overworld);
    level.getOrCreateDimension(VanillaDimensions::Nether);
    level.getOrCreateDimension(VanillaDimensions::TheEnd);
    auto add_dimension = [this](::Dimension &dimension) {
        dimensions_[dimension.getDimensionId().value] =
            std::make_shared<EndstoneDimension>(dimension.getWeakRef(), *this);
    };
    level.forEachDimension([&](::Dimension &dimension) {
        addDimension(dimension);
        return true;
    });
    level.getDimensionManager().getOnNewDimensionCreatedConnector().connect(
        [&](::Dimension &dimension) { addDimension(dimension); }, Bedrock::PubSub::ConnectPosition::AtBack, nullptr);
}

void EndstoneLevel::addDimension(::Dimension &dimension)
{
    dimensions_[dimension.getDimensionId().value] = std::make_unique<EndstoneDimension>(dimension.getWeakRef(), *this);
}

std::string EndstoneLevel::getName() const
{
    return level_.getLevelId();
}

std::vector<NotNull<Actor>> EndstoneLevel::getActors() const
{
    std::vector<NotNull<Actor>> result;
    for (const auto &entity : level_.getEntities()) {
        if (!entity.hasValue()) {
            continue;
        }
        const auto *actor = ::Actor::tryGetFromEntity(*entity, false);
        if (!actor) {
            continue;
        }
        if (&actor->getLevel() != &level_) {
            continue;
        }
        result.push_back(actor->getEndstoneActorPtr());
    }
    return result;
}

int EndstoneLevel::getTime() const
{
    return level_.getTime();
}

void EndstoneLevel::setTime(int time)
{
    level_.setTime(time);
}

std::vector<NotNull<Dimension>> EndstoneLevel::getDimensions() const
{
    std::vector<NotNull<Dimension>> dimensions;
    dimensions.reserve(dimensions_.size());
    for (const auto &val : dimensions_ | std::views::values) {
        dimensions.emplace_back(val);
    }
    return dimensions;
}

Nullable<Dimension> EndstoneLevel::getDimension(DimensionId id) const
{
    for (const auto &dimension : dimensions_ | std::views::values) {
        if (dimension->getId() == id) {
            return dimension;
        }
    }
    return nullptr;
}

Nullable<Dimension> EndstoneLevel::getDimension(DimensionType type) const
{
    if (const auto it = dimensions_.find(type.value); it != dimensions_.end()) {
        return it->second;
    }
    return nullptr;
}

Nullable<Dimension> EndstoneLevel::createDimension(const DimensionCreator & /*creator*/)
{
    // Deferred: serverRegisterCustomDimension and getDimensionId are inlined in the shipped BDS build, so there is no
    // symbol to call. Revisit once a callable registration path is wired.
    throw std::runtime_error("Level::createDimension is not yet implemented");
}

std::int64_t EndstoneLevel::getSeed() const
{
    return level_.getLevelSeed64();
}

bool EndstoneLevel::hasGameRule(Identifier<GameRule> rule) const
{
    if (rule.getNamespace() != Identifier<GameRule>::Minecraft) {
        return false;
    }
    return level_.getGameRules().getGameRule(std::string(rule.getKey())) != nullptr;
}

GameRuleValue EndstoneLevel::getGameRuleValue(Identifier<GameRule> rule) const
{
    if (rule.getNamespace() != Identifier<GameRule>::Minecraft) {
        throw std::out_of_range("Game rule is not available.");
    }

    const auto *game_rule = level_.getGameRules().getGameRule(std::string(rule.getKey()));
    if (game_rule == nullptr) {
        throw std::out_of_range("Game rule is not available.");
    }

    const auto &value = game_rule->getValue();
    switch (game_rule->getType()) {
    case ::GameRule::Type::Bool:
        return std::get<bool>(value);
    case ::GameRule::Type::Int:
        return std::get<int>(value);
    case ::GameRule::Type::Float:
        return std::get<float>(value);
    case ::GameRule::Type::Invalid:
        std::abort();
    }
}

bool EndstoneLevel::setGameRuleValue(Identifier<GameRule> rule, GameRuleValue value)
{
    if (rule.getNamespace() != Identifier<GameRule>::Minecraft) {
        return false;
    }

    auto &game_rules = level_.getGameRules();
    auto *game_rule = game_rules.getGameRule(std::string(rule.getKey()));
    if (game_rule == nullptr) {
        return false;
    }

    ::GameRule::Value native_value;
    switch (game_rule->getType()) {
    case ::GameRule::Type::Bool:
        if (!std::holds_alternative<bool>(value)) {
            return false;
        }
        native_value = std::get<bool>(value);
        break;
    case ::GameRule::Type::Int:
        if (!std::holds_alternative<int>(value)) {
            return false;
        }
        native_value = std::get<int>(value);
        break;
    case ::GameRule::Type::Float:
        if (!std::holds_alternative<float>(value)) {
            return false;
        }
        native_value = std::get<float>(value);
        break;
    case ::GameRule::Type::Invalid:
        return false;
    }

    bool validated = false;
    bool changed = false;
    const auto packet = game_rules.setGameRule(std::string(rule.getKey()), std::move(native_value), true, &validated,
                                               &changed, nullptr);
    if (!validated) {
        return false;
    }
    if (packet != nullptr) {
        level_.getPacketSender()->sendBroadcast(*packet);
    }
    return true;
}

EndstoneServer &EndstoneLevel::getServer() const
{
    return server_;
}

::Level &EndstoneLevel::getHandle() const
{
    return level_;
}

};  // namespace endstone::core
