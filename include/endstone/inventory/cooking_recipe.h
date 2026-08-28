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

#include <optional>

#include "endstone/inventory/recipe.h"

namespace endstone {
/**
 * Represents a cooking recipe.
 *
 * Bedrock records neither an experience reward nor a cooking time on the recipe itself, so neither is reported here.
 * The experience a smelt awards belongs to the input item, and the time a cook takes to the station.
 */
class CookingRecipe : public Recipe {
public:
    CookingRecipe(const CookingRecipe &other) = default;
    CookingRecipe(CookingRecipe &&other) noexcept = default;
    CookingRecipe &operator=(const CookingRecipe &other) = default;
    CookingRecipe &operator=(CookingRecipe &&other) noexcept = default;
    ~CookingRecipe() override = default;

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
     * Get the input choice of this recipe.
     *
     * @return the input ingredient
     */
    [[nodiscard]] std::optional<RecipeIngredient> getInputChoice() const { return getInput(); }

protected:
    friend class Recipe;
    friend class core::EndstoneRecipe;
    explicit CookingRecipe(std::unique_ptr<Impl> impl) : Recipe(std::move(impl)) {}
};
}  // namespace endstone
