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
    if (descriptor.getItem() == nullptr) {
        return {endstone::RecipeIngredientKind::Empty, {}, count, std::nullopt};
    }
    const auto identifier = descriptor.getFullName();
    const auto data = descriptor.getAuxValue();
    if (identifier.empty()) {
        return {endstone::RecipeIngredientKind::Unsupported,
                {},
                count,
                data == ItemDescriptor::ANY_AUX_VALUE ? std::nullopt : std::optional<int>(data)};
    }
    const auto type = descriptor.getType();
    if (type != ItemDescriptor::InternalType::Default && type != ItemDescriptor::InternalType::ItemTag) {
        return {endstone::RecipeIngredientKind::Unsupported, identifier, count,
                data == ItemDescriptor::ANY_AUX_VALUE ? std::nullopt : std::optional<int>(data)};
    }
    const auto kind = type == ItemDescriptor::InternalType::ItemTag ? endstone::RecipeIngredientKind::ItemTag
                                                                    : endstone::RecipeIngredientKind::Item;
    return {kind, identifier, count, data == ItemDescriptor::ANY_AUX_VALUE ? std::nullopt : std::optional<int>(data)};
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
    const auto &results = recipe->getResultItems();
    return results.empty() ? endstone::ItemType::get(endstone::ItemType::Air)->createItemStack(0)
                           : EndstoneItemStack::fromMinecraft(results.front());
}

const std::vector<endstone::RecipeIngredient> &EndstoneRecipeData::getIngredients() const
{
    if (ingredients_.empty() && !recipe->getIngredients().empty()) {
        ingredients_ = convertIngredients(recipe->getIngredients());
    }
    return ingredients_;
}

}  // namespace endstone::core
