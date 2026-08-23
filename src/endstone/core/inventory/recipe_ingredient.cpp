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

#include "endstone/core/inventory/recipe_ingredient.h"

#include "bedrock/world/item/item_descriptor.h"
#include "endstone/inventory/item_type.h"

namespace endstone::core {

Nullable<endstone::RecipeIngredient> makeIngredient(std::shared_ptr<const ::Recipe> recipe,
                                                    const ::RecipeIngredient &ingredient)
{
    const auto &descriptor = static_cast<const ::ItemDescriptor &>(ingredient);
    switch (descriptor.getType()) {
    case ItemDescriptor::InternalType::Invalid:
        return nullptr;
    case ItemDescriptor::InternalType::ItemTag:
    case ItemDescriptor::InternalType::ComplexAlias:
        return std::make_shared<EndstoneItemTagIngredient>(std::move(recipe), ingredient, descriptor.getFullName());
    case ItemDescriptor::InternalType::Default: {
        const auto *type = endstone::ItemType::get(descriptor.getFullName());
        if (descriptor.getItem() == nullptr || type == nullptr) {
            return nullptr;
        }
        const auto aux_value = descriptor.getAuxValue();
        if (aux_value == ItemDescriptor::ANY_AUX_VALUE) {
            return std::make_shared<EndstoneItemTypeIngredient>(std::move(recipe), ingredient, *type);
        }
        auto item = type->createItemStack(static_cast<int>(ingredient.getStackSize()));
        item.setData(aux_value);
        return std::make_shared<EndstoneExactIngredient>(std::move(recipe), ingredient, std::move(item));
    }
    default:
        return std::make_shared<EndstoneUnknownIngredient>(std::move(recipe), ingredient);
    }
}

}  // namespace endstone::core
