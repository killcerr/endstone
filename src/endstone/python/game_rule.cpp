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
namespace {
// The value type is carried by the stubs alone; at runtime a constant is its identifier.
auto constant(Identifier<GameRule> id)
{
    return [id](const py::object &) {
        return id;
    };
}
}  // namespace

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
        .def_static(
            "__class_getitem__", [](const py::object &) { return py::type::of<GameRule>(); }, py::arg("item"))
        .def_property_readonly_static("COMMAND_BLOCK_OUTPUT", constant(GameRule::CommandBlockOutput))
        .def_property_readonly_static("COMMAND_BLOCKS_ENABLED", constant(GameRule::CommandBlocksEnabled))
        .def_property_readonly_static("DO_DAY_LIGHT_CYCLE", constant(GameRule::DoDayLightCycle))
        .def_property_readonly_static("DO_ENTITY_DROPS", constant(GameRule::DoEntityDrops))
        .def_property_readonly_static("DO_FIRE_TICK", constant(GameRule::DoFireTick))
        .def_property_readonly_static("DO_IMMEDIATE_RESPAWN", constant(GameRule::DoImmediateRespawn))
        .def_property_readonly_static("DO_INSOMNIA", constant(GameRule::DoInsomnia))
        .def_property_readonly_static("DO_LIMITED_CRAFTING", constant(GameRule::DoLimitedCrafting))
        .def_property_readonly_static("DO_MOB_LOOT", constant(GameRule::DoMobLoot))
        .def_property_readonly_static("DO_MOB_SPAWNING", constant(GameRule::DoMobSpawning))
        .def_property_readonly_static("DO_TILE_DROPS", constant(GameRule::DoTileDrops))
        .def_property_readonly_static("DO_WEATHER_CYCLE", constant(GameRule::DoWeatherCycle))
        .def_property_readonly_static("DROWNING_DAMAGE", constant(GameRule::DrowningDamage))
        .def_property_readonly_static("FALL_DAMAGE", constant(GameRule::FallDamage))
        .def_property_readonly_static("FIRE_DAMAGE", constant(GameRule::FireDamage))
        .def_property_readonly_static("FREEZE_DAMAGE", constant(GameRule::FreezeDamage))
        .def_property_readonly_static("FUNCTION_COMMAND_LIMIT", constant(GameRule::FunctionCommandLimit))
        .def_property_readonly_static("KEEP_INVENTORY", constant(GameRule::KeepInventory))
        .def_property_readonly_static("LOCATOR_BAR", constant(GameRule::LocatorBar))
        .def_property_readonly_static("MAX_COMMAND_CHAIN_LENGTH", constant(GameRule::MaxCommandChainLength))
        .def_property_readonly_static("MOB_GRIEFING", constant(GameRule::MobGriefing))
        .def_property_readonly_static("NATURAL_REGENERATION", constant(GameRule::NaturalRegeneration))
        .def_property_readonly_static("PLAYERS_SLEEPING_PERCENTAGE", constant(GameRule::PlayersSleepingPercentage))
        .def_property_readonly_static("PLAYER_WAYPOINTS", constant(GameRule::PlayerWaypoints))
        .def_property_readonly_static("PROJECTILES_CAN_BREAK_BLOCKS", constant(GameRule::ProjectilesCanBreakBlocks))
        .def_property_readonly_static("PVP", constant(GameRule::Pvp))
        .def_property_readonly_static("RANDOM_TICK_SPEED", constant(GameRule::RandomTickSpeed))
        .def_property_readonly_static("RECIPES_UNLOCK", constant(GameRule::RecipesUnlock))
        .def_property_readonly_static("RESPAWN_BLOCKS_EXPLODE", constant(GameRule::RespawnBlocksExplode))
        .def_property_readonly_static("SEND_COMMAND_FEEDBACK", constant(GameRule::SendCommandFeedback))
        .def_property_readonly_static("SHOW_BORDER_EFFECT", constant(GameRule::ShowBorderEffect))
        .def_property_readonly_static("SHOW_COORDINATES", constant(GameRule::ShowCoordinates))
        .def_property_readonly_static("SHOW_DAYS_PLAYED", constant(GameRule::ShowDaysPlayed))
        .def_property_readonly_static("SHOW_DEATH_MESSAGES", constant(GameRule::ShowDeathMessages))
        .def_property_readonly_static("SHOW_RECIPE_MESSAGES", constant(GameRule::ShowRecipeMessages))
        .def_property_readonly_static("SHOW_TAGS", constant(GameRule::ShowTags))
        .def_property_readonly_static("SPAWN_RADIUS", constant(GameRule::SpawnRadius))
        .def_property_readonly_static("TNT_EXPLODES", constant(GameRule::TntExplodes))
        .def_property_readonly_static("TNT_EXPLOSION_DROP_DECAY", constant(GameRule::TntExplosionDropDecay));
}
}  // namespace endstone::python
