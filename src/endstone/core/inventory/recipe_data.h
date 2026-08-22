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
#include <vector>

#include "bedrock/world/item/crafting/recipe.h"
#include "endstone/inventory/item_stack.h"
#include "endstone/inventory/recipe_ingredient.h"

namespace endstone::core {

class EndstoneRecipeData {
public:
    explicit EndstoneRecipeData(std::shared_ptr<const ::Recipe> recipe) : recipe(std::move(recipe)) {}

    [[nodiscard]] endstone::ItemStack getResult() const;
    [[nodiscard]] const std::vector<endstone::RecipeIngredient> &getIngredients() const;
    [[nodiscard]] const ::Recipe &getHandle() const { return *recipe; }
    [[nodiscard]] bool isShapeless() const { return recipe->isShapeless(); }
    [[nodiscard]] const std::string &getRecipeId() const { return recipe->getRecipeId(); }
    [[nodiscard]] const std::string &getTag() const { return recipe->getTag().getString(); }

protected:
    const std::shared_ptr<const ::Recipe> recipe;
    mutable std::vector<endstone::RecipeIngredient> ingredients_;
};

}  // namespace endstone::core
