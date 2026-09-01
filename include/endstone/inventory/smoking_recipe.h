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
 * Represents a smoking recipe.
 */
class SmokingRecipe : public CookingRecipe {
public:
    SmokingRecipe(std::string recipe_id, RecipeIngredient input, ItemStack result, std::string tag = "smoker",
                  int priority = 0, RecipeUnlockingRequirement unlocking = RecipeUnlockingRequirement())
        : CookingRecipe(std::make_unique<SimpleImpl>(
              std::initializer_list<ClassInfo>{typeid(SmokingRecipe), typeid(CookingRecipe), typeid(Recipe)},
              std::move(recipe_id), std::move(tag), std::move(result),
              std::vector<std::optional<RecipeIngredient>>{std::move(input)}, priority, 0, 0, std::move(unlocking)))
    {
    }
    SmokingRecipe(const SmokingRecipe &other) = default;
    SmokingRecipe(SmokingRecipe &&other) noexcept = default;
    SmokingRecipe &operator=(const SmokingRecipe &other) = default;
    SmokingRecipe &operator=(SmokingRecipe &&other) noexcept = default;
    ~SmokingRecipe() override = default;

private:
    friend class Recipe;
    friend class core::EndstoneRecipe;
    explicit SmokingRecipe(std::unique_ptr<Impl> impl) : CookingRecipe(std::move(impl)) {}
};
}  // namespace endstone
