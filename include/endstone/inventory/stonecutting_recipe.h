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
#include <string>
#include <utility>
#include <vector>

#include "endstone/inventory/recipe.h"

namespace endstone {
/**
 * Represents a stonecutting recipe.
 */
class StonecuttingRecipe : public Recipe {
public:
    StonecuttingRecipe(std::string recipe_id, RecipeIngredient input, ItemStack result,
                       std::string tag = "stonecutter", int priority = 0)
        : Recipe(std::make_unique<SimpleImpl>(
              std::initializer_list<ClassInfo>{typeid(StonecuttingRecipe), typeid(Recipe)}, std::move(recipe_id),
              std::move(tag), std::move(result), std::vector<std::optional<RecipeIngredient>>{std::move(input)},
              priority))
    {
    }
    StonecuttingRecipe(const StonecuttingRecipe &other) = default;
    StonecuttingRecipe(StonecuttingRecipe &&other) noexcept = default;
    StonecuttingRecipe &operator=(const StonecuttingRecipe &other) = default;
    StonecuttingRecipe &operator=(StonecuttingRecipe &&other) noexcept = default;
    ~StonecuttingRecipe() override = default;

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

private:
    friend class Recipe;
    friend class core::EndstoneRecipe;
    explicit StonecuttingRecipe(std::unique_ptr<Impl> impl) : Recipe(std::move(impl)) {}
};
}  // namespace endstone
