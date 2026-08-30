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

#include "bedrock/world/item/alchemy/potion_brewing.h"

#include <cstdint>
#include <unordered_set>
#include <vector>

#include "bedrock/symbol.h"
#include "bedrock/world/item/registry/item_registry_manager.h"

PotionBrewing::Ingredient::Ingredient(const Item &item, const int data) : item_id_(item.getId()), data_(data) {}

PotionBrewing::Ingredient::Ingredient(const ItemInstance &item) : item_id_(item.getId()), data_(item.getAuxValue()) {}

bool PotionBrewing::Ingredient::operator==(const Ingredient &other) const
{
    return item_id_ == other.item_id_ && data_ == other.data_;
}

bool PotionBrewing::Ingredient::equals(const ItemDescriptor &input) const
{
    return item_id_ == input.getId() &&
           (data_ == Ingredient::AnyData || data_ == ItemDescriptor::ANY_AUX_VALUE || data_ == input.getAuxValue());
}

void PotionBrewing::addPotionMix(const ItemDescriptor &from, const Ingredient &ingredient, const ItemDescriptor &to)
{
    auto &mixes = *BEDROCK_VAR(std::vector<Mix<ItemDescriptor>> *, "PotionBrewing::mPotionMixes");
    for (const auto &mix : mixes) {
        if (mix.getFrom().sameItem(from, true) && mix.getIngredient() == ingredient &&
            mix.getTo().sameItem(to, true)) {
            return;
        }
    }
    mixes.emplace_back(from, ingredient, to);

    auto &valid_ingredients =
        *BEDROCK_VAR(std::unordered_set<Ingredient> *, "PotionBrewing::mValidIngredients");
    const auto reagent =
        ItemRegistryManager::getItemRegistry().getItem(static_cast<std::int16_t>(ingredient.getItemId()));
    if (reagent.isNull()) {
        valid_ingredients.insert(ingredient);
    }
    else {
        valid_ingredients.insert(ingredient);
        if (ingredient.getData() == Ingredient::AnyData || ingredient.getData() == ItemDescriptor::ANY_AUX_VALUE) {
            valid_ingredients.insert(Ingredient(*reagent, 0));
            valid_ingredients.insert(Ingredient(*reagent, ItemDescriptor::ANY_AUX_VALUE));
            valid_ingredients.insert(Ingredient(*reagent, Ingredient::AnyData));
        }
    }

    const auto *from_item = from.getItem();
    if (from_item == nullptr) {
        return;
    }
    const auto from_aux = from.getAuxValue();
    const Ingredient input(*from_item, from_aux == ItemDescriptor::ANY_AUX_VALUE ? Ingredient::AnyData : from_aux);
    auto &valid_recipe_inputs =
        *BEDROCK_VAR(std::unordered_set<Ingredient> *, "PotionBrewing::mValidRecipeInputs");
    valid_recipe_inputs.insert(input);
    if (input.getData() == Ingredient::AnyData || input.getData() == ItemDescriptor::ANY_AUX_VALUE) {
        valid_recipe_inputs.insert(Ingredient(*from_item, 0));
        valid_recipe_inputs.insert(Ingredient(*from_item, ItemDescriptor::ANY_AUX_VALUE));
        valid_recipe_inputs.insert(Ingredient(*from_item, Ingredient::AnyData));
    }
}

void PotionBrewing::addContainerRecipe(const Item &from, const Ingredient &ingredient, const Item &to)
{
    auto &mixes = *BEDROCK_VAR(std::vector<Mix<const Item &>> *, "PotionBrewing::mContainerMixes");
    for (const auto &mix : mixes) {
        if (mix.getFrom().getId() == from.getId() && mix.getIngredient() == ingredient &&
            mix.getTo().getId() == to.getId()) {
            return;
        }
    }
    mixes.emplace_back(from, ingredient, to);

    auto &valid_containers = *BEDROCK_VAR(std::vector<Ingredient> *, "PotionBrewing::mValidContainers");
    const Ingredient container = Ingredient(from, ItemDescriptor::ANY_AUX_VALUE);
    const Ingredient any_data = Ingredient(from, Ingredient::AnyData);
    const Ingredient zero_data = Ingredient(from, 0);
    for (const auto &variant : {container, any_data, zero_data}) {
        bool exists = false;
        for (const auto &valid : valid_containers) {
            if (valid == variant) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            valid_containers.push_back(variant);
        }
    }

    auto &valid_recipe_inputs =
        *BEDROCK_VAR(std::unordered_set<Ingredient> *, "PotionBrewing::mValidRecipeInputs");
    valid_recipe_inputs.insert(container);
    valid_recipe_inputs.insert(any_data);
    valid_recipe_inputs.insert(zero_data);
}

bool PotionBrewing::removePotionMix(const ItemDescriptor &from, const Ingredient &ingredient,
                                    const ItemDescriptor &to)
{
    auto &mixes = *BEDROCK_VAR(std::vector<Mix<ItemDescriptor>> *, "PotionBrewing::mPotionMixes");
    auto found = mixes.end();
    for (auto it = mixes.begin(); it != mixes.end(); ++it) {
        if (it->getFrom().sameItem(from, true) && it->getIngredient() == ingredient &&
            it->getTo().sameItem(to, true)) {
            found = it;
            break;
        }
    }
    if (found == mixes.end()) {
        return false;
    }
    mixes.erase(found);

    auto &valid_ingredients =
        *BEDROCK_VAR(std::unordered_set<Ingredient> *, "PotionBrewing::mValidIngredients");
    const auto reagent =
        ItemRegistryManager::getItemRegistry().getItem(static_cast<std::int16_t>(ingredient.getItemId()));
    if (!reagent.isNull()) {
        const Ingredient variants[] = {
            ingredient,
            Ingredient(*reagent, 0),
            Ingredient(*reagent, ItemDescriptor::ANY_AUX_VALUE),
            Ingredient(*reagent, Ingredient::AnyData),
        };
        for (const auto &variant : variants) {
            bool used = false;
            for (const auto &mix : mixes) {
                if (mix.getIngredient().getItemId() != variant.getItemId()) {
                    continue;
                }
                if (mix.getIngredient().getData() == Ingredient::AnyData ||
                    mix.getIngredient().getData() == ItemDescriptor::ANY_AUX_VALUE ||
                    variant.getData() == Ingredient::AnyData || variant.getData() == ItemDescriptor::ANY_AUX_VALUE ||
                    mix.getIngredient().getData() == variant.getData()) {
                    used = true;
                    break;
                }
            }
            if (!used) {
                valid_ingredients.erase(variant);
            }
        }
    }
    else {
        bool used = false;
        for (const auto &mix : mixes) {
            if (mix.getIngredient().getItemId() != ingredient.getItemId()) {
                continue;
            }
            if (mix.getIngredient().getData() == Ingredient::AnyData ||
                mix.getIngredient().getData() == ItemDescriptor::ANY_AUX_VALUE ||
                ingredient.getData() == Ingredient::AnyData || ingredient.getData() == ItemDescriptor::ANY_AUX_VALUE ||
                mix.getIngredient().getData() == ingredient.getData()) {
                used = true;
                break;
            }
        }
        if (!used) {
            valid_ingredients.erase(ingredient);
        }
    }

    const auto *from_item = from.getItem();
    if (from_item == nullptr) {
        return true;
    }
    const auto from_aux = from.getAuxValue();
    const Ingredient input(*from_item, from_aux == ItemDescriptor::ANY_AUX_VALUE ? Ingredient::AnyData : from_aux);
    const Ingredient variants[] = {
        input,
        Ingredient(*from_item, 0),
        Ingredient(*from_item, ItemDescriptor::ANY_AUX_VALUE),
        Ingredient(*from_item, Ingredient::AnyData),
    };
    auto &valid_recipe_inputs =
        *BEDROCK_VAR(std::unordered_set<Ingredient> *, "PotionBrewing::mValidRecipeInputs");
    for (const auto &variant : variants) {
        bool used = false;
        for (const auto &mix : mixes) {
            const auto *mix_item = mix.getFrom().getItem();
            if (mix_item == nullptr) {
                continue;
            }
            const auto mix_aux = mix.getFrom().getAuxValue();
            const Ingredient mix_input(*mix_item,
                                       mix_aux == ItemDescriptor::ANY_AUX_VALUE ? Ingredient::AnyData : mix_aux);
            if (mix_input.getItemId() != variant.getItemId()) {
                continue;
            }
            if (mix_input.getData() == Ingredient::AnyData || mix_input.getData() == ItemDescriptor::ANY_AUX_VALUE ||
                variant.getData() == Ingredient::AnyData || variant.getData() == ItemDescriptor::ANY_AUX_VALUE ||
                mix_input.getData() == variant.getData()) {
                used = true;
                break;
            }
        }
        if (!used) {
            valid_recipe_inputs.erase(variant);
        }
    }
    return true;
}

bool PotionBrewing::removeContainerRecipe(const Item &from, const Ingredient &ingredient, const Item &to)
{
    auto &mixes = *BEDROCK_VAR(std::vector<Mix<const Item &>> *, "PotionBrewing::mContainerMixes");
    std::vector<Mix<const Item &>> remaining;
    remaining.reserve(mixes.size());
    bool removed = false;
    for (const auto &mix : mixes) {
        if (!removed && mix.getFrom().getId() == from.getId() && mix.getIngredient() == ingredient &&
            mix.getTo().getId() == to.getId()) {
            removed = true;
            continue;
        }
        remaining.emplace_back(mix.getFrom(), mix.getIngredient(), mix.getTo());
    }
    if (!removed) {
        return false;
    }
    mixes.swap(remaining);

    auto &valid_containers = *BEDROCK_VAR(std::vector<Ingredient> *, "PotionBrewing::mValidContainers");
    const Ingredient variants[] = {
        Ingredient(from, ItemDescriptor::ANY_AUX_VALUE),
        Ingredient(from, Ingredient::AnyData),
        Ingredient(from, 0),
    };
    for (const auto &variant : variants) {
        bool used = false;
        for (const auto &mix : mixes) {
            if (mix.getFrom().getId() == variant.getItemId()) {
                used = true;
                break;
            }
        }
        if (!used) {
            for (auto it = valid_containers.begin(); it != valid_containers.end(); ++it) {
                if (*it == variant) {
                    valid_containers.erase(it);
                    break;
                }
            }
        }
    }

    auto &valid_recipe_inputs =
        *BEDROCK_VAR(std::unordered_set<Ingredient> *, "PotionBrewing::mValidRecipeInputs");
    for (const auto &variant : variants) {
        bool used = false;
        for (const auto &mix : mixes) {
            if (mix.getFrom().getId() == variant.getItemId()) {
                used = true;
                break;
            }
        }
        if (!used) {
            const auto potion_mixes =
                *BEDROCK_VAR(std::vector<Mix<ItemDescriptor>> *, "PotionBrewing::mPotionMixes");
            for (const auto &mix : potion_mixes) {
                const auto *mix_item = mix.getFrom().getItem();
                if (mix_item != nullptr && mix_item->getId() == variant.getItemId()) {
                    used = true;
                    break;
                }
            }
        }
        if (!used) {
            valid_recipe_inputs.erase(variant);
        }
    }
    return true;
}

const std::vector<PotionBrewing::Mix<ItemDescriptor>> &PotionBrewing::getPotionMixes()
{
    return *BEDROCK_VAR(std::vector<Mix<ItemDescriptor>> *, "PotionBrewing::mPotionMixes");
}

const std::vector<PotionBrewing::Mix<const Item &>> &PotionBrewing::getContainerMixes()
{
    return *BEDROCK_VAR(std::vector<Mix<const Item &>> *, "PotionBrewing::mContainerMixes");
}
