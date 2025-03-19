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

#include <iostream>

// We do not support compiling under MSVC Debug mode because it sets _ITERATOR_DEBUG_LEVEL
// to a nonzero value, changing the standard library's iterator implementation and resulting
// in an ABI incompatible with the BDS environment, which is built in Release mode.
#ifdef _MSC_VER
#include <iterator>
static_assert(_ITERATOR_DEBUG_LEVEL == 0,
              "Compilation aborted: _ITERATOR_DEBUG_LEVEL != 0. This indicates MSVC Debug mode, "
              "which is not supported. Please switch to a Release or RelWithDebInfo configuration.");
#endif

#include "actor/actor.h"
#include "actor/mob.h"
#include "ban/ban_entry.h"
#include "ban/ban_list.h"
#include "ban/ip_ban_entry.h"
#include "ban/ip_ban_list.h"
#include "ban/player_ban_entry.h"
#include "ban/player_ban_list.h"
#include "block/block.h"
#include "block/block_data.h"
#include "block/block_face.h"
#include "block/block_state.h"
#include "boss/bar_color.h"
#include "boss/bar_flag.h"
#include "boss/bar_style.h"
#include "boss/boss_bar.h"
#include "color_format.h"
#include "command/command.h"
#include "command/command_executor.h"
#include "command/command_map.h"
#include "command/command_sender.h"
#include "command/command_sender_wrapper.h"
#include "command/console_command_sender.h"
#include "command/plugin_command.h"
#include "command/proxied_command_sender.h"
#include "damage/damage_source.h"
#include "event/actor/actor_damage_event.h"
#include "event/actor/actor_death_event.h"
#include "event/actor/actor_event.h"
#include "event/actor/actor_explode_event.h"
#include "event/actor/actor_knockback_event.h"
#include "event/actor/actor_remove_event.h"
#include "event/actor/actor_spawn_event.h"
#include "event/actor/actor_teleport_event.h"
#include "event/block/block_break_event.h"
#include "event/block/block_event.h"
#include "event/block/block_place_event.h"
#include "event/cancellable.h"
#include "event/event.h"
#include "event/event_handler.h"
#include "event/event_priority.h"
#include "event/handler_list.h"
#include "event/player/player_chat_event.h"
#include "event/player/player_command_event.h"
#include "event/player/player_death_event.h"
#include "event/player/player_emote_event.h"
#include "event/player/player_event.h"
#include "event/player/player_game_mode_change_event.h"
#include "event/player/player_interact_actor_event.h"
#include "event/player/player_interact_event.h"
#include "event/player/player_join_event.h"
#include "event/player/player_kick_event.h"
#include "event/player/player_login_event.h"
#include "event/player/player_quit_event.h"
#include "event/player/player_respawn_event.h"
#include "event/player/player_teleport_event.h"
#include "event/server/broadcast_message_event.h"
#include "event/server/packet_receive_event.h"
#include "event/server/packet_send_event.h"
#include "event/server/plugin_disable_event.h"
#include "event/server/plugin_enable_event.h"
#include "event/server/script_message_event.h"
#include "event/server/server_command_event.h"
#include "event/server/server_event.h"
#include "event/server/server_list_ping_event.h"
#include "event/server/server_load_event.h"
#include "event/weather/thunder_change_event.h"
#include "event/weather/weather_change_event.h"
#include "event/weather/weather_event.h"
#include "form/action_form.h"
#include "form/controls/dropdown.h"
#include "form/controls/label.h"
#include "form/controls/slider.h"
#include "form/controls/step_slider.h"
#include "form/controls/text_input.h"
#include "form/controls/toggle.h"
#include "form/form.h"
#include "form/message_form.h"
#include "form/modal_form.h"
#include "game_mode.h"
#include "inventory/inventory.h"
#include "inventory/item_stack.h"
#include "inventory/meta/item_meta.h"
#include "inventory/player_inventory.h"
#include "lang/language.h"
#include "lang/translatable.h"
#include "level/chunk.h"
#include "level/dimension.h"
#include "level/level.h"
#include "level/location.h"
#include "level/position.h"
#include "logger.h"
#include "message.h"
#include "offline_player.h"
#include "permissions/permissible.h"
#include "permissions/permission.h"
#include "permissions/permission_attachment.h"
#include "permissions/permission_attachment_info.h"
#include "permissions/permission_default.h"
#include "player.h"
#include "plugin/plugin.h"
#include "plugin/plugin_description.h"
#include "plugin/plugin_load_order.h"
#include "plugin/plugin_loader.h"
#include "plugin/plugin_manager.h"
#include "plugin/service.h"
#include "plugin/service_manager.h"
#include "plugin/service_priority.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scoreboard/criteria.h"
#include "scoreboard/display_slot.h"
#include "scoreboard/objective.h"
#include "scoreboard/objective_sort_order.h"
#include "scoreboard/render_type.h"
#include "scoreboard/score.h"
#include "scoreboard/score_entry.h"
#include "scoreboard/scoreboard.h"
#include "server.h"
#include "skin.h"
#include "util/error.h"
#include "util/result.h"
#include "util/socket_address.h"
#include "util/uuid.h"
#include "util/vector.h"
#include "variant.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace endstone {
inline Server &getServer()
{
    static Server *server = nullptr;
    if (!server) {
        using GetServerFunc = Server &(*)();
#ifdef _WIN32
        const HMODULE module = GetModuleHandleA("endstone_runtime.dll");
        if (!module) {
            std::cerr << "Failed to obtain host module handle.\n";
            std::exit(1);
        }
        auto get_server = reinterpret_cast<GetServerFunc>(GetProcAddress(module, "endstone_getServer"));
#else
        // On Linux, use dlsym with RTLD_DEFAULT to search the global symbol table.
        auto get_server = reinterpret_cast<GetServerFunc>(dlsym(RTLD_DEFAULT, "endstone_getServer"));
#endif
        if (!get_server) {
            std::cerr << "Failed to retrieve getServer function pointer.\n";
            std::exit(1);
        }
        server = &get_server();
    }
    return *server;
}
}  // namespace endstone
