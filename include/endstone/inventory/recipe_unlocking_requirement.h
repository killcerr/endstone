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

#include <utility>
#include <vector>

#include "endstone/inventory/recipe_ingredient.h"

namespace endstone {
/**
 * How a recipe is unlocked in the recipe book.
 *
 * A recipe is unlocked either by a context, or by picking up one of a list of items. The two are exclusive: a
 * context-unlocked recipe has no unlocking ingredients, and an ingredient-unlocked recipe has context None.
 */
class RecipeUnlockingRequirement {
public:
    /**
     * A situation that unlocks a recipe without a specific item.
     */
    enum class UnlockingContext {
        /**
         * Not unlocked by a context. Used when the recipe is unlocked by ingredients instead.
         */
        None,
        /**
         * Unlocked as soon as the player joins.
         */
        AlwaysUnlocked,
        /**
         * Unlocked when the player is in water.
         */
        PlayerInWater,
        /**
         * Unlocked when the player is carrying many items.
         */
        PlayerHasManyItems,
    };

    /**
     * Creates a requirement unlocked by a context.
     *
     * Plugin recipes default to AlwaysUnlocked.
     *
     * @param context the unlocking context
     */
    explicit RecipeUnlockingRequirement(UnlockingContext context = UnlockingContext::AlwaysUnlocked)
        : context_(context)
    {
    }

    /**
     * Creates a requirement unlocked by picking up any of the given items.
     *
     * The context is None.
     *
     * @param unlocking_ingredients the items that unlock this recipe
     */
    explicit RecipeUnlockingRequirement(std::vector<RecipeIngredient> unlocking_ingredients)
        : ingredients_(std::move(unlocking_ingredients))
    {
    }

    /**
     * Whether this recipe can be unlocked at all.
     *
     * @return true if a context other than None is set, or unlocking ingredients are present
     */
    [[nodiscard]] bool isUnlockable() const
    {
        return context_ != UnlockingContext::None || !ingredients_.empty();
    }

    /**
     * Whether this recipe is unlocked by a context rather than by items.
     *
     * @return true if a context other than None is set
     */
    [[nodiscard]] bool isUnlockedByContext() const { return context_ != UnlockingContext::None; }

    /**
     * Whether this recipe is unlocked by picking up items.
     *
     * @return true if unlocking ingredients are present
     */
    [[nodiscard]] bool isUnlockedByIngredients() const { return !ingredients_.empty(); }

    /**
     * The context that unlocks this recipe.
     *
     * @return the unlocking context, or None when unlocked by ingredients
     */
    [[nodiscard]] UnlockingContext getContext() const { return context_; }

    /**
     * The items that unlock this recipe.
     *
     * Empty when unlocked by a context.
     *
     * @return the unlocking ingredients
     */
    [[nodiscard]] const std::vector<RecipeIngredient> &getIngredients() const { return ingredients_; }

private:
    UnlockingContext context_{UnlockingContext::None};
    std::vector<RecipeIngredient> ingredients_;
};
}  // namespace endstone
