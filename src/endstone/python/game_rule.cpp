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

#include "endstone_python.h"

namespace py = pybind11;

namespace endstone::python {
void init_game_rule(py::module_ &m)
{
    py::class_<GameRule>(m, "GameRule", "Represents a game rule.")
        .def_property_readonly("id", &GameRule::getId, "The identifier of this game rule.")
        .def_property_readonly("translation_key", &GameRule::getTranslationKey,
                               "The translation key, suitable for use in a translation component.")
        .def_static("get", &GameRule::get, py::arg("name"), R"doc(
    Attempts to get the `GameRule` with the given name.

    Args:
        name: The identifier of the game rule (e.g. `minecraft:dofiretick`).

    Returns:
        The `GameRule`, or `None` if no game rule with that name exists.
)doc",
                    py::return_value_policy::reference)
        .def("__str__", [](const GameRule &self) { return std::string(self.getId()); })
        .def("__repr__", [](const GameRule &self) { return std::format("GameRule({})", self.getId()); })
        .def("__hash__", [](const GameRule &self) { return py::hash(py::str(std::string(self.getId()))); })
        .def_property_readonly_static("COMMAND_BLOCK_OUTPUT",
                                      [](const py::object &) { return GameRule::CommandBlockOutput; })
        .def_property_readonly_static("DO_DAY_LIGHT_CYCLE",
                                      [](const py::object &) { return GameRule::DoDayLightCycle; })
        .def_property_readonly_static("DO_ENTITY_DROPS", [](const py::object &) { return GameRule::DoEntityDrops; })
        .def_property_readonly_static("DO_FIRE_TICK", [](const py::object &) { return GameRule::DoFireTick; })
        .def_property_readonly_static("RECIPES_UNLOCK", [](const py::object &) { return GameRule::RecipesUnlock; })
        .def_property_readonly_static("DO_LIMITED_CRAFTING",
                                      [](const py::object &) { return GameRule::DoLimitedCrafting; })
        .def_property_readonly_static("DO_MOB_LOOT", [](const py::object &) { return GameRule::DoMobLoot; })
        .def_property_readonly_static("DO_MOB_SPAWNING", [](const py::object &) { return GameRule::DoMobSpawning; })
        .def_property_readonly_static("DO_TILE_DROPS", [](const py::object &) { return GameRule::DoTileDrops; })
        .def_property_readonly_static("DO_WEATHER_CYCLE", [](const py::object &) { return GameRule::DoWeatherCycle; })
        .def_property_readonly_static("DROWNING_DAMAGE", [](const py::object &) { return GameRule::DrowningDamage; })
        .def_property_readonly_static("FALL_DAMAGE", [](const py::object &) { return GameRule::FallDamage; })
        .def_property_readonly_static("FIRE_DAMAGE", [](const py::object &) { return GameRule::FireDamage; })
        .def_property_readonly_static("KEEP_INVENTORY", [](const py::object &) { return GameRule::KeepInventory; })
        .def_property_readonly_static("LOCATOR_BAR", [](const py::object &) { return GameRule::LocatorBar; })
        .def_property_readonly_static("MOB_GRIEFING", [](const py::object &) { return GameRule::MobGriefing; })
        .def_property_readonly_static("PVP", [](const py::object &) { return GameRule::Pvp; })
        .def_property_readonly_static("SHOW_COORDINATES", [](const py::object &) { return GameRule::ShowCoordinates; })
        .def_property_readonly_static("PLAYER_WAYPOINTS", [](const py::object &) { return GameRule::PlayerWaypoints; })
        .def_property_readonly_static("SHOW_DAYS_PLAYED", [](const py::object &) { return GameRule::ShowDaysPlayed; })
        .def_property_readonly_static("NATURAL_REGENERATION",
                                      [](const py::object &) { return GameRule::NaturalRegeneration; })
        .def_property_readonly_static("TNT_EXPLODES", [](const py::object &) { return GameRule::TntExplodes; })
        .def_property_readonly_static("SEND_COMMAND_FEEDBACK",
                                      [](const py::object &) { return GameRule::SendCommandFeedback; })
        .def_property_readonly_static("MAX_COMMAND_CHAIN_LENGTH",
                                      [](const py::object &) { return GameRule::MaxCommandChainLength; })
        .def_property_readonly_static("DO_INSOMNIA", [](const py::object &) { return GameRule::DoInsomnia; })
        .def_property_readonly_static("COMMAND_BLOCKS_ENABLED",
                                      [](const py::object &) { return GameRule::CommandBlocksEnabled; })
        .def_property_readonly_static("RANDOM_TICK_SPEED", [](const py::object &) { return GameRule::RandomTickSpeed; })
        .def_property_readonly_static("DO_IMMEDIATE_RESPAWN",
                                      [](const py::object &) { return GameRule::DoImmediateRespawn; })
        .def_property_readonly_static("SHOW_DEATH_MESSAGES",
                                      [](const py::object &) { return GameRule::ShowDeathMessages; })
        .def_property_readonly_static("FUNCTION_COMMAND_LIMIT",
                                      [](const py::object &) { return GameRule::FunctionCommandLimit; })
        .def_property_readonly_static("SPAWN_RADIUS", [](const py::object &) { return GameRule::SpawnRadius; })
        .def_property_readonly_static("SHOW_TAGS", [](const py::object &) { return GameRule::ShowTags; })
        .def_property_readonly_static("FREEZE_DAMAGE", [](const py::object &) { return GameRule::FreezeDamage; })
        .def_property_readonly_static("RESPAWN_BLOCKS_EXPLODE",
                                      [](const py::object &) { return GameRule::RespawnBlocksExplode; })
        .def_property_readonly_static("SHOW_BORDER_EFFECT",
                                      [](const py::object &) { return GameRule::ShowBorderEffect; })
        .def_property_readonly_static("SHOW_RECIPE_MESSAGES",
                                      [](const py::object &) { return GameRule::ShowRecipeMessages; })
        .def_property_readonly_static("PLAYERS_SLEEPING_PERCENTAGE",
                                      [](const py::object &) { return GameRule::PlayersSleepingPercentage; })
        .def_property_readonly_static("PROJECTILES_CAN_BREAK_BLOCKS",
                                      [](const py::object &) { return GameRule::ProjectilesCanBreakBlocks; })
        .def_property_readonly_static("TNT_EXPLOSION_DROP_DECAY",
                                      [](const py::object &) { return GameRule::TntExplosionDropDecay; });
}
}  // namespace endstone::python
