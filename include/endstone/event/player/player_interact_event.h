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

#include "endstone/event/player/player_event.h"

namespace endstone {

/**
 * @brief Represents an event that is called when a player right-clicks a block.
 */
class PlayerInteractEvent : public PlayerEvent {
public:
    PlayerInteractEvent(Player &player, Block &block_clicked, BlockFace block_face,
                        const Vector<float> &clicked_position)
        : PlayerEvent(player), block_clicked_(block_clicked), block_face_(block_face),
          clicked_position_(clicked_position)
    {
    }
    ~PlayerInteractEvent() override = default;

    inline static const std::string NAME = "PlayerInteractEvent";
    [[nodiscard]] std::string getEventName() const override
    {
        return NAME;
    }

    [[nodiscard]] bool isCancellable() const override
    {
        return true;
    }

    /**
     * @brief Returns the clicked block
     *
     * @return Block returns the block clicked with this item.
     */
    [[nodiscard]] Block &getBlockClicked() const
    {
        return block_clicked_;
    }

    /**
     * @brief Returns the face of the block that was clicked
     *
     * @return BlockFace returns the face of the block that was clicked
     */
    [[nodiscard]] BlockFace getBlockFace() const
    {
        return block_face_;
    }

    /**
     * @brief Gets the exact position on the block the player interacted with.
     * <p>
     * All vector components are between 0.0 and 1.0 inclusive.
     *
     * @return the clicked position.
     */
    [[nodiscard]] Vector<float> getClickedPosition() const
    {
        return clicked_position_;
    }

private:
    // TODO(item): add ItemStack item
    Block &block_clicked_;
    BlockFace block_face_;
    Vector<float> clicked_position_;
};

}  // namespace endstone