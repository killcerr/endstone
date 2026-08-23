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

#include <cstddef>

#include "endstone/core/inventory/recipe_data.h"
#include "endstone/inventory/smithing_recipe.h"

namespace endstone::core {

class EndstoneSmithingRecipe final : public EndstoneRecipeBase<endstone::SmithingRecipe> {
public:
    using EndstoneRecipeBase::EndstoneRecipeBase;

    [[nodiscard]] endstone::RecipeIngredient getTemplateIngredient() const override { return getIngredient(0); }
    [[nodiscard]] endstone::RecipeIngredient getBaseIngredient() const override { return getIngredient(1); }
    [[nodiscard]] endstone::RecipeIngredient getAdditionIngredient() const override { return getIngredient(2); }

private:
    [[nodiscard]] endstone::RecipeIngredient getIngredient(std::size_t index) const
    {
        const auto &ingredients = EndstoneRecipeData::getIngredients();
        return index < ingredients.size() ? ingredients[index] : endstone::RecipeIngredient{};
    }
};

}  // namespace endstone::core
