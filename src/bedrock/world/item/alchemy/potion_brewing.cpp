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

#include <algorithm>
#include <cstddef>
#include <optional>
#include <unordered_set>
#include <vector>

#include "bedrock/symbol.h"
#include "bedrock/world/item/registry/item_registry_manager.h"

namespace {

using IngredientSet = std::unordered_set<PotionBrewing::Ingredient>;
using IngredientVector = std::vector<PotionBrewing::Ingredient>;

constexpr std::size_t ValidIngredientsOffset = 0x48;
constexpr std::size_t ValidRecipeInputsOffset = 0x88;
constexpr std::size_t ValidContainersOffset = 0xC8;

std::byte *potionBrewingStorage()
{
    static auto *storage = BEDROCK_VAR(std::byte *, "PotionBrewing::mPotionMixes");
    return storage;
}

template <typename T>
T &storageAt(const std::size_t offset)
{
    return *reinterpret_cast<T *>(potionBrewingStorage() + offset);
}

IngredientSet &validIngredients()
{
    return storageAt<IngredientSet>(ValidIngredientsOffset);
}

IngredientSet &validRecipeInputs()
{
    return storageAt<IngredientSet>(ValidRecipeInputsOffset);
}

IngredientVector &validContainers()
{
    return storageAt<IngredientVector>(ValidContainersOffset);
}

std::vector<PotionBrewing::Mix<ItemDescriptor>> &potionMixes()
{
    return storageAt<std::vector<PotionBrewing::Mix<ItemDescriptor>>>(0);
}

std::vector<PotionBrewing::Mix<const Item &>> &containerMixes()
{
    return storageAt<std::vector<PotionBrewing::Mix<const Item &>>>(0x18);
}

const Item *itemById(int id)
{
    auto item = ItemRegistryManager::getItemRegistry().getItem(static_cast<std::int16_t>(id));
    return item.isNull() ? nullptr : item.get();
}

std::optional<PotionBrewing::Ingredient> inputIngredient(const ItemDescriptor &from)
{
    const auto *item = from.getItem();
    if (item == nullptr) {
        return std::nullopt;
    }
    const auto aux = from.getAuxValue();
    return PotionBrewing::Ingredient(
        *item, aux == ItemDescriptor::ANY_AUX_VALUE ? PotionBrewing::Ingredient::AnyData : aux);
}

PotionBrewing::Ingredient containerIngredient(const Item &item)
{
    return {item, ItemDescriptor::ANY_AUX_VALUE};
}

bool sameSlot(const PotionBrewing::Ingredient &lhs, const PotionBrewing::Ingredient &rhs)
{
    if (lhs.getItemId() != rhs.getItemId()) {
        return false;
    }
    const auto lhs_any = lhs.getData() == PotionBrewing::Ingredient::AnyData ||
                         lhs.getData() == ItemDescriptor::ANY_AUX_VALUE;
    const auto rhs_any = rhs.getData() == PotionBrewing::Ingredient::AnyData ||
                         rhs.getData() == ItemDescriptor::ANY_AUX_VALUE;
    return lhs_any || rhs_any || lhs.getData() == rhs.getData();
}

bool potionUsesIngredient(const PotionBrewing::Ingredient &ingredient)
{
    return std::ranges::any_of(potionMixes(), [&](const PotionBrewing::Mix<ItemDescriptor> &mix) {
        return sameSlot(mix.getIngredient(), ingredient);
    });
}

bool potionUsesInput(const PotionBrewing::Ingredient &ingredient)
{
    return std::ranges::any_of(potionMixes(), [&](const PotionBrewing::Mix<ItemDescriptor> &mix) {
        const auto input = inputIngredient(mix.getFrom());
        return input && sameSlot(*input, ingredient);
    });
}

bool containerUsesInput(const PotionBrewing::Ingredient &ingredient)
{
    return std::ranges::any_of(containerMixes(), [&](const PotionBrewing::Mix<const Item &> &mix) {
        return sameSlot(containerIngredient(mix.getFrom()), ingredient);
    });
}

void insertVariants(std::unordered_set<PotionBrewing::Ingredient> &set, const Item &item,
                    const PotionBrewing::Ingredient &ingredient)
{
    set.insert(ingredient);
    if (ingredient.getData() == PotionBrewing::Ingredient::AnyData ||
        ingredient.getData() == ItemDescriptor::ANY_AUX_VALUE) {
        set.insert({item, 0});
        set.insert({item, ItemDescriptor::ANY_AUX_VALUE});
        set.insert({item, PotionBrewing::Ingredient::AnyData});
    }
}

void eraseUnused(std::unordered_set<PotionBrewing::Ingredient> &set, const Item &item,
                 const PotionBrewing::Ingredient &ingredient, auto still_used)
{
    const PotionBrewing::Ingredient variants[] = {
        ingredient,
        {item, 0},
        {item, ItemDescriptor::ANY_AUX_VALUE},
        {item, PotionBrewing::Ingredient::AnyData},
    };
    for (const auto &variant : variants) {
        if (!still_used(variant)) {
            set.erase(variant);
        }
    }
}

void rememberPotion(const ItemDescriptor &from, const PotionBrewing::Ingredient &ingredient)
{
    auto &valid_ingredients = validIngredients();
    if (const auto *reagent = itemById(ingredient.getItemId())) {
        insertVariants(valid_ingredients, *reagent, ingredient);
    }
    else {
        valid_ingredients.insert(ingredient);
    }

    const auto input = inputIngredient(from);
    if (!input) {
        return;
    }
    auto &valid_recipe_inputs = validRecipeInputs();
    if (const auto *item = from.getItem()) {
        insertVariants(valid_recipe_inputs, *item, *input);
    }
    else {
        valid_recipe_inputs.insert(*input);
    }
}

void forgetPotion(const ItemDescriptor &from, const PotionBrewing::Ingredient &ingredient)
{
    auto &valid_ingredients = validIngredients();
    if (const auto *reagent = itemById(ingredient.getItemId())) {
        eraseUnused(valid_ingredients, *reagent, ingredient, potionUsesIngredient);
    }
    else if (!potionUsesIngredient(ingredient)) {
        valid_ingredients.erase(ingredient);
    }

    const auto input = inputIngredient(from);
    if (!input) {
        return;
    }
    auto &valid_recipe_inputs = validRecipeInputs();
    if (const auto *item = from.getItem()) {
        eraseUnused(valid_recipe_inputs, *item, *input, potionUsesInput);
    }
    else if (!potionUsesInput(*input)) {
        valid_recipe_inputs.erase(*input);
    }
}

void rememberContainer(const Item &from)
{
    auto &valid_containers = validContainers();
    const PotionBrewing::Ingredient variants[] = {
        containerIngredient(from),
        {from, PotionBrewing::Ingredient::AnyData},
        {from, 0},
    };
    for (const auto &variant : variants) {
        if (std::ranges::find(valid_containers, variant) == valid_containers.end()) {
            valid_containers.push_back(variant);
        }
    }
    insertVariants(validRecipeInputs(), from, variants[0]);
}

void forgetContainer(const Item &from)
{
    auto &valid_containers = validContainers();
    const PotionBrewing::Ingredient variants[] = {
        containerIngredient(from),
        {from, PotionBrewing::Ingredient::AnyData},
        {from, 0},
    };
    for (const auto &variant : variants) {
        if (containerUsesInput(variant) || potionUsesInput(variant)) {
            continue;
        }
        if (const auto found = std::ranges::find(valid_containers, variant); found != valid_containers.end()) {
            valid_containers.erase(found);
        }
    }
    eraseUnused(validRecipeInputs(), from, variants[0], [](const PotionBrewing::Ingredient &ingredient) {
        return containerUsesInput(ingredient) || potionUsesInput(ingredient);
    });
}

}  // namespace

PotionBrewing::Ingredient::Ingredient(const Item &item, int data) : item_id_(item.getId()), data_(data) {}

PotionBrewing::Ingredient::Ingredient(const ItemInstance &item) : item_id_(item.getId()), data_(item.getAuxValue()) {}

bool PotionBrewing::Ingredient::operator==(const Ingredient &other) const
{
    return item_id_ == other.item_id_ && data_ == other.data_;
}

bool PotionBrewing::Ingredient::equals(const ItemDescriptor &input) const
{
    if (item_id_ != input.getId()) {
        return false;
    }
    return data_ == Ingredient::AnyData || data_ == ItemDescriptor::ANY_AUX_VALUE || data_ == input.getAuxValue();
}

void PotionBrewing::addPotionMix(const ItemDescriptor &from, const Ingredient &ingredient, const ItemDescriptor &to)
{
    auto &mixes = potionMixes();
    const auto duplicate = std::ranges::find_if(mixes, [&](const Mix<ItemDescriptor> &mix) {
        return mix.getFrom().sameItem(from, true) && mix.getIngredient() == ingredient &&
               mix.getTo().sameItem(to, true);
    });
    if (duplicate != mixes.end()) {
        return;
    }
    mixes.emplace_back(from, ingredient, to);
    rememberPotion(from, ingredient);
}

void PotionBrewing::addContainerRecipe(const Item &from, const Ingredient &ingredient, const Item &to)
{
    auto &mixes = containerMixes();
    const auto duplicate = std::ranges::find_if(mixes, [&](const Mix<const Item &> &mix) {
        return mix.getFrom().getId() == from.getId() && mix.getIngredient() == ingredient &&
               mix.getTo().getId() == to.getId();
    });
    if (duplicate != mixes.end()) {
        return;
    }
    mixes.emplace_back(from, ingredient, to);
    rememberContainer(from);
}

bool PotionBrewing::removePotionMix(const ItemDescriptor &from, const Ingredient &ingredient,
                                    const ItemDescriptor &to)
{
    auto &mixes = potionMixes();
    const auto found = std::ranges::find_if(mixes, [&](const Mix<ItemDescriptor> &mix) {
        return mix.getFrom().sameItem(from, true) && mix.getIngredient() == ingredient &&
               mix.getTo().sameItem(to, true);
    });
    if (found == mixes.end()) {
        return false;
    }
    mixes.erase(found);
    forgetPotion(from, ingredient);
    return true;
}

bool PotionBrewing::removeContainerRecipe(const Item &from, const Ingredient &ingredient, const Item &to)
{
    auto &mixes = containerMixes();
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
    forgetContainer(from);
    return true;
}

const std::vector<PotionBrewing::Mix<ItemDescriptor>> &PotionBrewing::getPotionMixes()
{
    return potionMixes();
}

const std::vector<PotionBrewing::Mix<const Item &>> &PotionBrewing::getContainerMixes()
{
    return containerMixes();
}
