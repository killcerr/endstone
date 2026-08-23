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

#include "bedrock/world/item/item_descriptor.h"
#include "endstone/core/inventory/item_stack.h"

namespace endstone::core {
namespace {

endstone::RecipeIngredient convertIngredient(const ::RecipeIngredient &ingredient)
{
    const auto &descriptor = static_cast<const ::ItemDescriptor &>(ingredient);
    const auto count = static_cast<int>(ingredient.getStackSize());
    switch (descriptor.getType()) {
    case ItemDescriptor::InternalType::Default: {
        if (descriptor.getItem() == nullptr) {
            return {endstone::RecipeIngredientKind::Empty, {}, count, std::nullopt};
        }
        const auto aux_value = descriptor.getAuxValue();
        return {endstone::RecipeIngredientKind::Item, descriptor.getFullName(), count,
                aux_value == ItemDescriptor::ANY_AUX_VALUE ? std::nullopt : std::optional<int>(aux_value)};
    }
    case ItemDescriptor::InternalType::ItemTag:
        return {endstone::RecipeIngredientKind::ItemTag, descriptor.getFullName(), count, std::nullopt};
    case ItemDescriptor::InternalType::Invalid:
        return {endstone::RecipeIngredientKind::Empty, {}, count, std::nullopt};
    default:
        return {endstone::RecipeIngredientKind::Unsupported, descriptor.getFullName(), count, std::nullopt};
    }
}
std::vector<endstone::RecipeIngredient> convertIngredients(const std::vector<::RecipeIngredient> &ingredients)
{
    std::vector<endstone::RecipeIngredient> result;
    result.reserve(ingredients.size());
    for (const auto &ingredient : ingredients) {
        result.push_back(convertIngredient(ingredient));
    }
    return result;
}

}  // namespace

endstone::ItemStack EndstoneRecipeData::getResult() const
{
    const auto &results = recipe_->getResultItems();
    return results.empty() ? endstone::ItemType::get(endstone::ItemType::Air)->createItemStack(0)
                           : EndstoneItemStack::fromMinecraft(results.front());
}

const std::vector<endstone::RecipeIngredient> &EndstoneRecipeData::getIngredients() const
{
    if (ingredients_.empty() && !recipe_->getIngredients().empty()) {
        ingredients_ = convertIngredients(recipe_->getIngredients());
    }
    return ingredients_;
}

}  // namespace endstone::core
