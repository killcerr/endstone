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

#include "endstone/core/inventory/recipe.h"

#include "endstone/core/inventory/multi_recipe.h"
#include "endstone/core/inventory/shaped_recipe.h"
#include "endstone/core/inventory/shapeless_recipe.h"
#include "endstone/core/inventory/smithing_recipe.h"

namespace endstone::core {

NotNull<endstone::Recipe> makeRecipe(std::shared_ptr<const ::Recipe> recipe)
{
    if (recipe->isMultiRecipe()) {
        return std::make_shared<EndstoneMultiRecipe>(std::move(recipe));
    }
    if (recipe->getTag().getString() == "smithing_table") {
        return std::make_shared<EndstoneSmithingRecipe>(std::move(recipe));
    }
    if (recipe->isShapeless()) {
        return std::make_shared<EndstoneShapelessRecipe>(std::move(recipe));
    }
    return std::make_shared<EndstoneShapedRecipe>(std::move(recipe));
}

std::vector<NotNull<endstone::Recipe>> makeRecipes(const ::Recipes &recipes)
{
    std::vector<NotNull<endstone::Recipe>> result;
    for (const auto &by_tag : recipes.getRecipesAllTags()) {
        for (const auto &by_id : by_tag.second) {
            if (by_id.second) {
                result.push_back(makeRecipe(by_id.second));
            }
        }
    }
    return result;
}

}  // namespace endstone::core
