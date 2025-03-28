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
#include <set>

#include "endstone/inventory/recipes/recipe.h"
#include "endstone/inventory/recipes/recipe_choice.h"

namespace endstone {
class ShapelessRecipe : public Recipe {
public:
    struct Ingredient {
        std::shared_ptr<RecipeChoice> choice;
        mutable std::uint16_t count;
        bool operator<(const Ingredient &other) const
        {
            return choice < other.choice;
        }
    };
    ~ShapelessRecipe() override = default;
    ShapelessRecipe(std::string recipe_id, std::shared_ptr<ItemStack> result) : Recipe(std::move(recipe_id))
    {
        result_.emplace_back(std::move(result));
    }
    std::vector<std::shared_ptr<ItemStack>> &getResult() override
    {
        return result_;
    }
    ShapelessRecipe &addIngredient(const std::shared_ptr<RecipeChoice> &choice, std::uint16_t count)
    {
        if (ingredients_.contains({choice, count})) {
            ingredients_.find({choice, count})->count += count;
        }
        return *this;
    }
    RecipeType getType() override
    {
        return RecipeType::Shapeless;
    }
    std::set<Ingredient> getIngredients()
    {
        return ingredients_;
    }

private:
    std::vector<std::shared_ptr<ItemStack>> result_;
    std::set<Ingredient> ingredients_;
};
}  // namespace endstone
