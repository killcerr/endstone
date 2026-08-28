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

#include <string>
#include <utility>
#include <vector>

#include "endstone/inventory/brewing_recipe.h"

namespace endstone {
/**
 * Represents a brewing container recipe.
 *
 * The reagent changes the bottle, such as a potion into a splash potion, keeping the potion inside it.
 */
class BrewingContainerRecipe : public BrewingRecipe {
public:
    BrewingContainerRecipe(std::string recipe_id, RecipeIngredient input, RecipeIngredient reagent, ItemStack result,
                           std::string tag = "brewing_stand")
        : BrewingRecipe(std::make_unique<SimpleImpl>(
              std::initializer_list<ClassInfo>{typeid(BrewingContainerRecipe), typeid(BrewingRecipe), typeid(Recipe)},
              std::move(recipe_id), std::move(tag), std::move(result),
              std::vector<std::optional<RecipeIngredient>>{std::move(input), std::move(reagent)}))
    {
    }
    BrewingContainerRecipe(const BrewingContainerRecipe &other) = default;
    BrewingContainerRecipe(BrewingContainerRecipe &&other) noexcept = default;
    BrewingContainerRecipe &operator=(const BrewingContainerRecipe &other) = default;
    BrewingContainerRecipe &operator=(BrewingContainerRecipe &&other) noexcept = default;
    ~BrewingContainerRecipe() override = default;

private:
    friend class Recipe;
    friend class BrewingRecipe;
    friend class core::EndstoneRecipe;
    explicit BrewingContainerRecipe(std::unique_ptr<Impl> impl) : BrewingRecipe(std::move(impl)) {}
};
}  // namespace endstone
