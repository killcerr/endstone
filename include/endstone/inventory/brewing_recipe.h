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

#include "endstone/inventory/recipe.h"

namespace endstone {
/**
 * Represents a brewing-stand recipe.
 *
 * Mix recipes change the potion in the bottle. Container recipes change the bottle itself, such as a potion into a
 * splash potion.
 */
class BrewingRecipe : public Recipe {
public:
    BrewingRecipe(const BrewingRecipe &other) = default;
    BrewingRecipe(BrewingRecipe &&other) noexcept = default;
    BrewingRecipe &operator=(const BrewingRecipe &other) = default;
    BrewingRecipe &operator=(BrewingRecipe &&other) noexcept = default;
    ~BrewingRecipe() override = default;

    /**
     * Get the input this recipe consumes.
     *
     * @return the input ingredient
     */
    [[nodiscard]] std::optional<RecipeIngredient> getInput() const
    {
        const auto &ingredients = getIngredients();
        return ingredients.empty() ? std::nullopt : ingredients.front();
    }

    /**
     * Get the reagent this recipe consumes.
     *
     * @return the reagent ingredient
     */
    [[nodiscard]] std::optional<RecipeIngredient> getReagent() const
    {
        const auto &ingredients = getIngredients();
        return ingredients.size() < 2 ? std::nullopt : ingredients[1];
    }

protected:
    friend class BrewingMixRecipe;
    friend class BrewingContainerRecipe;
    friend class core::EndstoneRecipe;
    explicit BrewingRecipe(std::unique_ptr<Impl> impl) : Recipe(std::move(impl)) {}
};
}  // namespace endstone
