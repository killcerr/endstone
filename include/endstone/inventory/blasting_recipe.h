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

#include "endstone/inventory/cooking_recipe.h"

namespace endstone {
/**
 * Represents a blasting recipe.
 */
class BlastingRecipe : public CookingRecipe {
public:
    BlastingRecipe(std::string recipe_id, RecipeIngredient input, ItemStack result, std::string tag = "blast_furnace",
                   int priority = 0, RecipeUnlockingRequirement unlocking = RecipeUnlockingRequirement())
        : CookingRecipe(std::make_unique<SimpleImpl>(
              std::initializer_list<ClassInfo>{typeid(BlastingRecipe), typeid(CookingRecipe), typeid(Recipe)},
              std::move(recipe_id), std::move(tag), std::move(result),
              std::vector<std::optional<RecipeIngredient>>{std::move(input)}, priority, 0, 0, std::move(unlocking)))
    {
    }
    BlastingRecipe(const BlastingRecipe &other) = default;
    BlastingRecipe(BlastingRecipe &&other) noexcept = default;
    BlastingRecipe &operator=(const BlastingRecipe &other) = default;
    BlastingRecipe &operator=(BlastingRecipe &&other) noexcept = default;
    ~BlastingRecipe() override = default;

private:
    friend class Recipe;
    friend class core::EndstoneRecipe;
    explicit BlastingRecipe(std::unique_ptr<Impl> impl) : CookingRecipe(std::move(impl)) {}
};
}  // namespace endstone
