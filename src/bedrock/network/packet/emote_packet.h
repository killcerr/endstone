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

#include "bedrock/network/packet.h"
#include "bedrock/world/actor/actor_runtime_id.h"

class EmotePacket : public Packet {
public:
    enum class Flags : uint8_t {
        SERVER_SIDE = 1,
        MUTE_EMOTE_CHAT = 2,
    };

    EmotePacket();
    EmotePacket(ActorRuntimeID, const std::string &, uint32_t, const std::string &, const std::string &);
    void setServerSide();
    [[nodiscard]] bool isServerSide() const
    {
        return (flags & static_cast<uint8_t>(Flags::SERVER_SIDE)) != 0;
    }
    void setEmoteChatMute();
    [[nodiscard]] bool isEmoteChatMuted() const
    {
        return (flags & static_cast<uint8_t>(Flags::MUTE_EMOTE_CHAT)) != 0;
    }
    ActorRuntimeID runtime_id;
    std::string piece_id;
    uint32_t emote_ticks;
    std::string xuid;
    std::string platform_id;
    uint8_t flags;
};
