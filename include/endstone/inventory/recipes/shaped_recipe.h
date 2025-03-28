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
#include <map>

#include "endstone/inventory/recipes/recipe.h"
#include "endstone/inventory/recipes/recipe_choice.h"

namespace endstone {
class ShapedRecipe : public Recipe {
public:
    ~ShapedRecipe() override = default;
    ShapedRecipe(std::string recipe_id, std::vector<ItemStack> result)
        : Recipe(std::move(recipe_id)), result_(std::move(result))
    {
    }
    std::vector<ItemStack> &getResult() override
    {
        return result_;
    }
    void shape(std::vector<std::string> rows)
    {
        rows_ = std::move(rows);
    }
    ShapedRecipe &setIngredient(char key, std::unique_ptr<RecipeChoice> choice)
    {
        ingredients_.emplace(key, choice);
        return *this;
    }

private:
    std::vector<ItemStack> result_;
    std::vector<std::string> rows_;
    std::map<char, RecipeChoice> ingredients_;
};
}  // namespace endstone
