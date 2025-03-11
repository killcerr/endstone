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

namespace endstone {

/**
 * @brief Called when the server receives a packet from a connected client.
 */
class PacketReceiveEvent : public Cancellable<ServerEvent> {
public:
    PacketReceiveEvent(Player &player, int packet_id, std::string_view payload)
        : player_(player), packet_id_(packet_id), payload_(payload)
    {
    }

    inline static const std::string NAME = "PacketReceiveEvent";
    [[nodiscard]] std::string getEventName() const override
    {
        return NAME;
    }

    /**
     * @brief Gets the ID of the packet.
     *
     * @return The packet ID.
     */
    [[nodiscard]] int getPacketId() const
    {
        return packet_id_;
    }

    /**
     * @brief Gets the raw packet data **excluding** the header.
     *
     * @return The packet payload data.
     */
    [[nodiscard]] std::string_view getPayload() const
    {
        return payload_;
    }

    /**
     * Returns the player involved in this event
     *
     * @return Player who is involved in this event
     */
    [[nodiscard]] Player &getPlayer() const
    {
        return player_;
    }

private:
    Player &player_;
    int packet_id_;
    std::string_view payload_;
};

}  // namespace endstone
