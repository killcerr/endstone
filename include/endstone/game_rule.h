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

#include <type_traits>
#include <variant>

#include "endstone/identifier.h"

namespace endstone {

using GameRuleValue = std::variant<bool, int, float>;

template <typename T>
class GameRuleId : public Identifier<class GameRule> {
public:
    using Identifier::Identifier;

    static constexpr GameRuleId minecraft(const std::string_view key) noexcept { return {Minecraft, key}; }
};

/**
 * All game rules.
 */
class GameRule {
public:
    /** Whether command blocks notify administrators when they execute commands. */
    static constexpr auto CommandBlockOutput = GameRuleId<bool>::minecraft("commandBlockOutput");
    /** Whether command blocks can execute commands. */
    static constexpr auto CommandBlocksEnabled = GameRuleId<bool>::minecraft("commandBlocksEnabled");
    /** Whether the day and night cycles progress. */
    static constexpr auto DoDayLightCycle = GameRuleId<bool>::minecraft("doDayLightCycle");
    /** Whether non-mob entities drop items. */
    static constexpr auto DoEntityDrops = GameRuleId<bool>::minecraft("doEntityDrops");
    /** Whether fire spreads. */
    static constexpr auto DoFireTick = GameRuleId<bool>::minecraft("doFireTick");
    /** Whether players immediately respawn. */
    static constexpr auto DoImmediateRespawn = GameRuleId<bool>::minecraft("doImmediateRespawn");
    /** Whether players experience insomnia. */
    static constexpr auto DoInsomnia = GameRuleId<bool>::minecraft("doInsomnia");
    /** Whether players can craft only unlocked recipes. */
    static constexpr auto DoLimitedCrafting = GameRuleId<bool>::minecraft("doLimitedCrafting");
    /** Whether mobs drop loot. */
    static constexpr auto DoMobLoot = GameRuleId<bool>::minecraft("doMobLoot");
    /** Whether mobs spawn naturally. */
    static constexpr auto DoMobSpawning = GameRuleId<bool>::minecraft("doMobSpawning");
    /** Whether blocks drop items when destroyed. */
    static constexpr auto DoTileDrops = GameRuleId<bool>::minecraft("doTileDrops");
    /** Whether the weather changes naturally. */
    static constexpr auto DoWeatherCycle = GameRuleId<bool>::minecraft("doWeatherCycle");
    /** Whether entities take drowning damage. */
    static constexpr auto DrowningDamage = GameRuleId<bool>::minecraft("drowningDamage");
    /** Whether entities take fall damage. */
    static constexpr auto FallDamage = GameRuleId<bool>::minecraft("fallDamage");
    /** Whether entities take fire damage. */
    static constexpr auto FireDamage = GameRuleId<bool>::minecraft("fireDamage");
    /** Whether entities take freezing damage. */
    static constexpr auto FreezeDamage = GameRuleId<bool>::minecraft("freezeDamage");
    /** The maximum number of commands executed by a function per tick. */
    static constexpr auto FunctionCommandLimit = GameRuleId<int>::minecraft("functionCommandLimit");
    /** Whether players keep their inventory on death. */
    static constexpr auto KeepInventory = GameRuleId<bool>::minecraft("keepInventory");
    /** Whether the locator bar is displayed. */
    static constexpr auto LocatorBar = GameRuleId<bool>::minecraft("locatorbar");
    /** The maximum number of chained commands executed per tick. */
    static constexpr auto MaxCommandChainLength = GameRuleId<int>::minecraft("maxCommandChainLength");
    /** Whether mobs can modify the world. */
    static constexpr auto MobGriefing = GameRuleId<bool>::minecraft("mobGriefing");
    /** Whether players regenerate health naturally. */
    static constexpr auto NaturalRegeneration = GameRuleId<bool>::minecraft("naturalRegeneration");
    /** The percentage of players that must sleep to skip the night. */
    static constexpr auto PlayersSleepingPercentage = GameRuleId<int>::minecraft("playersSleepingPercentage");
    /** Whether player waypoints are added to locator bars. */
    static constexpr auto PlayerWaypoints = GameRuleId<int>::minecraft("playerWaypoints");
    /** Whether projectiles can break supported blocks. */
    static constexpr auto ProjectilesCanBreakBlocks = GameRuleId<bool>::minecraft("projectilesCanBreakBlocks");
    /** Whether players can damage each other. */
    static constexpr auto Pvp = GameRuleId<bool>::minecraft("pvp");
    /** The frequency of random block ticks. */
    static constexpr auto RandomTickSpeed = GameRuleId<int>::minecraft("randomTickSpeed");
    /** Whether built-in recipes unlock as players progress. */
    static constexpr auto RecipesUnlock = GameRuleId<bool>::minecraft("recipesUnlock");
    /** Whether respawn blocks explode in other dimensions. */
    static constexpr auto RespawnBlocksExplode = GameRuleId<bool>::minecraft("respawnBlocksExplode");
    /** Whether command feedback is displayed to players. */
    static constexpr auto SendCommandFeedback = GameRuleId<bool>::minecraft("sendCommandFeedback");
    /** Whether border block effects are shown. */
    static constexpr auto ShowBorderEffect = GameRuleId<bool>::minecraft("showBorderEffect");
    /** Whether player coordinates are displayed. */
    static constexpr auto ShowCoordinates = GameRuleId<bool>::minecraft("showCoordinates");
    /** Whether the number of days played is displayed. */
    static constexpr auto ShowDaysPlayed = GameRuleId<bool>::minecraft("showDaysPlayed");
    /** Whether death messages are displayed in chat. */
    static constexpr auto ShowDeathMessages = GameRuleId<bool>::minecraft("showDeathMessages");
    /** Whether recipe unlock notifications are shown. */
    static constexpr auto ShowRecipeMessages = GameRuleId<bool>::minecraft("showRecipeMessages");
    /** Whether item tags are shown. */
    static constexpr auto ShowTags = GameRuleId<bool>::minecraft("showTags");
    /** The radius around world spawn in which players can spawn. */
    static constexpr auto SpawnRadius = GameRuleId<int>::minecraft("spawnRadius");
    /** Whether TNT blocks can be lit. */
    static constexpr auto TntExplodes = GameRuleId<bool>::minecraft("tntExplodes");
    /** Whether explosion drop decay is enabled for TNT. */
    static constexpr auto TntExplosionDropDecay = GameRuleId<bool>::minecraft("tntExplosionDropDecay");
};

static_assert(std::is_same_v<decltype(GameRule::DoFireTick), const GameRuleId<bool>>);
static_assert(std::is_same_v<decltype(GameRule::PlayerWaypoints), const GameRuleId<int>>);
static_assert(std::is_same_v<decltype(GameRule::RandomTickSpeed), const GameRuleId<int>>);

}  // namespace endstone
