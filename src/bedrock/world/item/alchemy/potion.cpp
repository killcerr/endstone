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

#include "bedrock/world/item/alchemy/potion.h"

#include "bedrock/symbol.h"

int Potion::getPotionId() const
{
    return id_;
}

std::string Potion::getNameId() const
{
    return name_id_;
}

std::string Potion::getDescriptionId() const
{
    if (description_ids_.empty()) {
        return {};
    }
    return description_ids_.front();
}

std::vector<std::shared_ptr<const Potion>> Potion::getPotions()
{
    // Endstone: Potion::mPotionsById[MAX_POTIONS] is the BDS registry global, indexed by potion id.
    static auto *potions = BEDROCK_VAR(std::shared_ptr<const Potion> *, "Potion::mPotionsById");
    std::vector<std::shared_ptr<const Potion>> result;
    for (int i = 0; i < MAX_POTIONS; ++i) {
        if (potions[i]) {
            result.push_back(potions[i]);
        }
    }
    return result;
}
