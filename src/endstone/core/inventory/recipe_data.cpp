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

#include "endstone/core/inventory/recipe_data.h"

#include "endstone/core/inventory/item_stack.h"
#include "endstone/core/inventory/recipe_ingredient.h"
#include "endstone/inventory/item_type.h"

namespace endstone::core {

endstone::ItemStack EndstoneRecipeData::getResult() const
{
    const auto &results = recipe_->getResultItems();
    return results.empty() ? endstone::ItemType::get(endstone::ItemType::Air)->createItemStack(0)
                           : EndstoneItemStack::fromMinecraft(results.front());
}

const std::vector<Nullable<endstone::RecipeIngredient>> &EndstoneRecipeData::getIngredients() const
{
    if (!ingredients_built_) {
        const auto &ingredients = recipe_->getIngredients();
        ingredients_.reserve(ingredients.size());
        for (const auto &ingredient : ingredients) {
            ingredients_.push_back(makeIngredient(recipe_, ingredient));
        }
        ingredients_built_ = true;
    }
    return ingredients_;
}

Nullable<endstone::RecipeIngredient> EndstoneRecipeData::getIngredient(const std::size_t index) const
{
    const auto &ingredients = getIngredients();
    return index < ingredients.size() ? ingredients[index] : nullptr;
}

}  // namespace endstone::core
