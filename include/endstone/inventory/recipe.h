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
#include <vector>

#include "endstone/inventory/item_stack.h"
#include "endstone/inventory/recipe_ingredient.h"

namespace endstone {
/** Identifies the concrete kind of a crafting recipe. */
enum class RecipeType {
    Shaped,
    Shapeless,
    Smithing,
    Multi,
};

/** Represents some type of crafting recipe. */
class Recipe {
public:
    virtual ~Recipe() = default;
    /** Gets the concrete kind of this recipe. */
    [[nodiscard]] virtual RecipeType getType() const noexcept = 0;
    /** Gets the result of this recipe. */
    [[nodiscard]] virtual ItemStack getResult() const = 0;
    /** Gets the ingredients used by this recipe. */
    [[nodiscard]] virtual const std::vector<RecipeIngredient> &getIngredients() const = 0;
    /** Gets the string identifier assigned to this recipe. */
    [[nodiscard]] virtual const std::string &getRecipeId() const = 0;
    /** Gets the crafting tag assigned to this recipe. */
    [[nodiscard]] virtual const std::string &getTag() const = 0;
};
}  // namespace endstone
