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

#include <stdexcept>

#include "bedrock/network/packet.h"
#include "bedrock/network/packet/cerealize/core/serialization_mode.h"
#include "bedrock/world/level/block_pos.h"

struct OpenSignPacketPayload {
    BlockPos pos;
    bool is_front_side;
};
BEDROCK_STATIC_ASSERT_SIZE(OpenSignPacketPayload, 16, 16);

class OpenSignPacket : public Packet {
    [[nodiscard]] MinecraftPacketIds getId() const override { return MinecraftPacketIds::OpenSign; }
    [[nodiscard]] std::string_view getName() const override { return "OpenSignPacket"; }
    void write(BinaryStream & /*unused*/) const override { throw std::logic_error("Not implemented"); }

protected:
    Bedrock::Result<void> _read(ReadOnlyBinaryStream & /*unused*/) override
    {
        throw std::logic_error("Not implemented");
    }

public:
    static const bool SHARE_WITH_HANDLER = false;

    OpenSignPacketPayload payload;
    SerializationMode serialization_mode;
};
BEDROCK_STATIC_ASSERT_SIZE(OpenSignPacket, 72, 64);
