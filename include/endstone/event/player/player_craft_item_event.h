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

#include <memory>
#include <utility>

#include "endstone/event/cancellable.h"
#include "endstone/event/player/player_event.h"
#include "endstone/inventory/item_stack.h"
#include "endstone/inventory/recipe.h"

namespace endstone {

/**
 * Called when a player crafts an item.
 *
 * @note If the event is cancelled the item will not be crafted and the ingredients will not be consumed.
 */
class PlayerCraftItemEvent final : public Cancellable<PlayerEvent> {
public:
    ENDSTONE_EVENT(PlayerCraftItemEvent);

    PlayerCraftItemEvent(const NotNull<Player> &player, ItemStack item, std::unique_ptr<Recipe> recipe, int amount)
        : Cancellable(player), item_(std::move(item)), recipe_(std::move(recipe)), amount_(amount)
    {
    }

    /**
     * Gets the item that is being crafted.
     *
     * @return an ItemStack for the item being crafted
     */
    [[nodiscard]] const ItemStack &getItem() const { return item_; }

    /**
     * Gets the recipe being crafted.
     *
     * @return the recipe
     */
    [[nodiscard]] const Recipe &getRecipe() const { return *recipe_; }

    /**
     * Gets the number of times the recipe is being crafted.
     *
     * @return the number of crafts
     */
    [[nodiscard]] int getAmount() const { return amount_; }

private:
    ItemStack item_;
    std::unique_ptr<Recipe> recipe_;
    int amount_;
};

}  // namespace endstone
