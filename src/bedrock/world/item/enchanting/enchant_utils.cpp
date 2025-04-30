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

#include "bedrock/world/item/enchanting/enchant_utils.h"

#include "bedrock/symbol.h"

bool EnchantUtils::applyEnchant(::ItemStackBase &out, ::Enchant::Type type, int level, bool allow_non_vanilla)
{
    return BEDROCK_CALL(&EnchantUtils::applyEnchant, out, type, level, allow_non_vanilla);
}

bool EnchantUtils::hasEnchant(::Enchant::Type type, const ::ItemStackBase &item)
{
    if (!item.hasUserData()) {
        return false;
    }
    const auto *user_data = item.getUserData();
    if (!user_data->contains("ench")) {
        return false;
    }
    const auto *ench = user_data->getList("ench");
    for (std::size_t i = 0; i < ench->size(); i++) {
        if (ench->getCompound(static_cast<int>(i))->getShort("id") == static_cast<unsigned char>(type)) {
            return true;
        }
    }
    return false;
}
