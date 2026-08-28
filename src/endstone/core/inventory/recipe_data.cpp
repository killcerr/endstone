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

#include <algorithm>
#include <format>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

#include "bedrock/world/item/alchemy/potion_brewing.h"
#include "bedrock/world/item/crafting/shaped_recipe.h"
#include "bedrock/world/item/crafting/shapeless_recipe.h"
#include "bedrock/world/item/crafting/smithing_transform_recipe.h"
#include "bedrock/world/item/crafting/smithing_trim_recipe.h"
#include "bedrock/world/item/item.h"
#include "bedrock/world/item/item_instance.h"
#include "bedrock/world/item/item_stack.h"
#include "bedrock/world/item/registry/item_registry_manager.h"
#include "bedrock/world/level/block/furnace_types.h"
#include "endstone/core/inventory/item_stack.h"
#include "endstone/core/inventory/recipe_ingredient.h"
#include "endstone/inventory/item_type.h"

namespace endstone::core {

endstone::Recipe EndstoneRecipe::fromMinecraft(std::shared_ptr<const ::Recipe> recipe)
{
    if (recipe->isMultiRecipe()) {
        return ComplexRecipe(std::make_unique<EndstoneComplexRecipe>(std::move(recipe)));
    }
    if (recipe->getTag().getString() == "smithing_table") {
        if (recipe->hasDataDrivenResult()) {
            return SmithingTransformRecipe(std::make_unique<EndstoneSmithingTransformRecipe>(std::move(recipe)));
        }
        return SmithingTrimRecipe(std::make_unique<EndstoneSmithingTrimRecipe>(std::move(recipe)));
    }
    if (recipe->isShapeless()) {
        const auto &tag = recipe->getTag();
        if (tag == FURNACE_TAG || tag == BLAST_FURNACE_TAG || tag == SMOKER_TAG || tag == CAMPFIRE_TAG ||
            tag == SOUL_CAMPFIRE_TAG) {
            return FurnaceRecipe(std::make_unique<EndstoneFurnaceRecipe>(std::move(recipe)));
        }
        return ShapelessRecipe(std::make_unique<EndstoneShapelessRecipe>(std::move(recipe)));
    }
    return ShapedRecipe(std::make_unique<EndstoneShapedRecipe>(std::move(recipe)));
}

std::optional<endstone::Recipe> EndstoneRecipe::fromMinecraft(const ::Recipes &recipes, const ::Recipe &recipe)
{
    const auto &all_tags = recipes.getRecipesAllTags();
    const auto by_tag = all_tags.find(recipe.getTag());
    if (by_tag == all_tags.end()) {
        return std::nullopt;
    }
    const auto by_id = by_tag->second.find(recipe.getRecipeId());
    if (by_id == by_tag->second.end()) {
        return std::nullopt;
    }
    return fromMinecraft(by_id->second);
}

namespace {

::Recipe::Ingredients toMinecraftIngredients(const std::vector<std::optional<endstone::RecipeIngredient>> &ingredients)
{
    ::Recipe::Ingredients slots;
    slots.reserve(ingredients.size());
    for (const auto &ingredient : ingredients) {
        if (ingredient) {
            slots.push_back(EndstoneIngredient::toMinecraft(*ingredient));
        }
        else {
            slots.emplace_back();
        }
    }
    return slots;
}

::Recipe::Results toMinecraftResults(const endstone::ItemStack &result)
{
    return ::Recipe::Results({ItemInstance(EndstoneItemStack::toMinecraft(result))});
}

RecipeUnlockingRequirement alwaysUnlocked()
{
    return RecipeUnlockingRequirement(RecipeUnlockingRequirement::UnlockingContext::AlwaysUnlocked);
}

ItemDescriptor descriptorFrom(const RecipeIngredient &ingredient)
{
    return EndstoneIngredient::toMinecraft(ingredient);
}

ItemDescriptor descriptorFrom(const ItemStack &stack)
{
    const auto minecraft = EndstoneItemStack::toMinecraft(stack);
    const auto *item = minecraft.getItem();
    if (item == nullptr) {
        return ItemDescriptor();
    }
    return ItemDescriptor(*item, minecraft.getAuxValue());
}

std::optional<PotionBrewing::Ingredient> brewingIngredientFrom(const RecipeIngredient &ingredient)
{
    const auto minecraft = EndstoneIngredient::toMinecraft(ingredient);
    const auto *item = minecraft.getItem();
    if (item == nullptr) {
        return std::nullopt;
    }
    const auto aux = minecraft.getAuxValue();
    return PotionBrewing::Ingredient(
        *item, aux == ItemDescriptor::ANY_AUX_VALUE ? PotionBrewing::Ingredient::AnyData : aux);
}

endstone::ItemStack stackFrom(const ItemDescriptor &descriptor)
{
    const auto *item = descriptor.getItem();
    if (item == nullptr) {
        return endstone::ItemType::get(endstone::ItemType::Air)->createItemStack(0);
    }
    return EndstoneItemStack::fromMinecraft(::ItemStack(*item, 1, descriptor.getAuxValue()));
}

endstone::ItemStack stackFrom(const ::Item &item)
{
    return EndstoneItemStack::fromMinecraft(::ItemStack(item, 1));
}

std::optional<RecipeIngredient> ingredientFrom(const ItemDescriptor &descriptor)
{
    return EndstoneIngredient::fromMinecraft(nullptr, ::RecipeIngredient(descriptor, 1));
}

std::optional<RecipeIngredient> ingredientFrom(const PotionBrewing::Ingredient &ingredient)
{
    auto item = ItemRegistryManager::getItemRegistry().getItem(static_cast<std::int16_t>(ingredient.getItemId()));
    if (item.isNull()) {
        return std::nullopt;
    }
    const auto aux = ingredient.getData() == PotionBrewing::Ingredient::AnyData ? ItemDescriptor::ANY_AUX_VALUE
                                                                                : ingredient.getData();
    return EndstoneIngredient::fromMinecraft(nullptr, ::RecipeIngredient(*item, aux, 1));
}

std::string mixId(const PotionBrewing::Mix<ItemDescriptor> &mix)
{
    return std::format("minecraft:brewing_mix/{}:{}+{}:{}/{}:{}", mix.getFrom().getFullName(),
                       mix.getFrom().getAuxValue(), mix.getIngredient().getItemId(), mix.getIngredient().getData(),
                       mix.getTo().getFullName(), mix.getTo().getAuxValue());
}

std::string containerId(const PotionBrewing::Mix<const ::Item &> &mix)
{
    return std::format("minecraft:brewing_container/{}/{}:{}/{}", mix.getFrom().getFullItemName(),
                       mix.getIngredient().getItemId(), mix.getIngredient().getData(), mix.getTo().getFullItemName());
}

struct PluginMix {
    ItemDescriptor from;
    PotionBrewing::Ingredient reagent;
    ItemDescriptor to;
};

struct PluginContainer {
    const ::Item *from;
    PotionBrewing::Ingredient reagent;
    const ::Item *to;
};

std::unordered_map<std::string, PluginMix> &pluginMixes()
{
    static std::unordered_map<std::string, PluginMix> mixes;
    return mixes;
}

std::unordered_map<std::string, PluginContainer> &pluginContainers()
{
    static std::unordered_map<std::string, PluginContainer> mixes;
    return mixes;
}

bool sameMix(const PotionBrewing::Mix<ItemDescriptor> &mix, const PluginMix &plugin)
{
    return mix.getFrom().sameItem(plugin.from, true) && mix.getIngredient() == plugin.reagent &&
           mix.getTo().sameItem(plugin.to, true);
}

bool sameContainer(const PotionBrewing::Mix<const ::Item &> &mix, const PluginContainer &plugin)
{
    return mix.getFrom().getId() == plugin.from->getId() && mix.getIngredient() == plugin.reagent &&
           mix.getTo().getId() == plugin.to->getId();
}
bool sameMixContent(const ItemDescriptor &from, const PotionBrewing::Ingredient &reagent, const ItemDescriptor &to,
                    const PluginMix &plugin)
{
    return from.sameItem(plugin.from, true) && reagent == plugin.reagent && to.sameItem(plugin.to, true);
}

bool sameContainerContent(const ::Item &from, const PotionBrewing::Ingredient &reagent, const ::Item &to,
                          const PluginContainer &plugin)
{
    return from.getId() == plugin.from->getId() && reagent == plugin.reagent && to.getId() == plugin.to->getId();
}

std::string mixContentKey(const ItemDescriptor &from, const PotionBrewing::Ingredient &reagent,
                          const ItemDescriptor &to)
{
    return std::format("{}:{}+{}:{}/{}:{}", from.getFullName(), from.getAuxValue(), reagent.getItemId(),
                       reagent.getData(), to.getFullName(), to.getAuxValue());
}

std::string containerContentKey(const ::Item &from, const PotionBrewing::Ingredient &reagent, const ::Item &to)
{
    return std::format("{}/{}:{}/{}", from.getFullItemName(), reagent.getItemId(), reagent.getData(),
                       to.getFullItemName());
}

std::unordered_set<std::string> &pluginAddedMixKeys()
{
    static std::unordered_set<std::string> keys;
    return keys;
}

std::unordered_set<std::string> &pluginAddedContainerKeys()
{
    static std::unordered_set<std::string> keys;
    return keys;
}

}  // namespace

std::unique_ptr<::Recipe> EndstoneRecipe::toMinecraft(const Recipe &recipe)
{
    if (recipe.is<BrewingRecipe>()) {
        return nullptr;
    }
    if (recipe.impl_->isEndstoneRecipe()) {
        const auto &handle = static_cast<const EndstoneRecipe &>(*recipe.impl_).getHandle();
        auto context = handle.getConstructionContext();
        if (handle.getTag().getString() == "smithing_table") {
            const auto &ingredients = handle.getIngredients();
            if (ingredients.size() < 3) {
                return nullptr;
            }
            if (handle.hasDataDrivenResult()) {
                return std::make_unique<::SmithingTransformRecipe>(handle.getRecipeId(), ingredients[0], ingredients[1],
                                                                   ingredients[2], std::move(context.results),
                                                                   handle.getTag());
            }
            return std::make_unique<::SmithingTrimRecipe>(handle.getRecipeId(), ingredients[0], ingredients[1],
                                                          ingredients[2], handle.getTag());
        }
        if (handle.isShapeless()) {
            return std::make_unique<::ShapelessRecipe>(std::move(context));
        }
        return std::make_unique<::ShapedRecipe>(std::move(context), handle.getWidth(), handle.getHeight(), true);
    }

    if (recipe.is<ComplexRecipe>()) {
        return nullptr;
    }

    if (const auto transform = recipe.as<SmithingTransformRecipe>()) {
        const auto template_ingredient = transform->getTemplate();
        const auto base = transform->getBase();
        const auto addition = transform->getAddition();
        if (!template_ingredient || !base || !addition) {
            return nullptr;
        }
        auto handle = std::make_unique<::SmithingTransformRecipe>(
            transform->getRecipeId(), EndstoneIngredient::toMinecraft(*template_ingredient),
            EndstoneIngredient::toMinecraft(*base), EndstoneIngredient::toMinecraft(*addition),
            toMinecraftResults(transform->getResult()), HashedString(transform->getTag()));
        handle->setUnlockingRequirement(alwaysUnlocked());
        return handle;
    }

    if (const auto trim = recipe.as<SmithingTrimRecipe>()) {
        const auto template_ingredient = trim->getTemplate();
        const auto base = trim->getBase();
        const auto addition = trim->getAddition();
        if (!template_ingredient || !base || !addition) {
            return nullptr;
        }
        auto handle = std::make_unique<::SmithingTrimRecipe>(
            trim->getRecipeId(), EndstoneIngredient::toMinecraft(*template_ingredient),
            EndstoneIngredient::toMinecraft(*base), EndstoneIngredient::toMinecraft(*addition),
            HashedString(trim->getTag()));
        handle->setUnlockingRequirement(alwaysUnlocked());
        return handle;
    }

    if (const auto furnace = recipe.as<FurnaceRecipe>()) {
        ::Recipe::ConstructionContext context;
        context.recipe_id = furnace->getRecipeId();
        context.ingredients = toMinecraftIngredients(furnace->getIngredients());
        context.results = toMinecraftResults(furnace->getResult());
        context.tag = HashedString(furnace->getTag());
        context.unlocking_requirement = alwaysUnlocked();
        return std::make_unique<::ShapelessRecipe>(std::move(context));
    }

    if (const auto shapeless = recipe.as<ShapelessRecipe>()) {
        ::Recipe::ConstructionContext context;
        context.recipe_id = shapeless->getRecipeId();
        context.ingredients = toMinecraftIngredients(shapeless->getIngredients());
        context.results = toMinecraftResults(shapeless->getResult());
        context.tag = HashedString(shapeless->getTag());
        context.unlocking_requirement = alwaysUnlocked();
        return std::make_unique<::ShapelessRecipe>(std::move(context));
    }

    if (const auto shaped = recipe.as<ShapedRecipe>()) {
        ::Recipe::ConstructionContext context;
        context.recipe_id = shaped->getRecipeId();
        context.ingredients = toMinecraftIngredients(shaped->getIngredients());
        context.results = toMinecraftResults(shaped->getResult());
        context.tag = HashedString(shaped->getTag());
        context.unlocking_requirement = alwaysUnlocked();
        return std::make_unique<::ShapedRecipe>(std::move(context), shaped->getWidth(), shaped->getHeight(), true);
    }

    return nullptr;
}

endstone::ItemStack EndstoneRecipe::getResult() const
{
    const auto &results = recipe_->getResultItems();
    return results.empty() ? endstone::ItemType::get(endstone::ItemType::Air)->createItemStack(0)
                           : EndstoneItemStack::fromMinecraft(results.front());
}

const std::vector<std::optional<endstone::RecipeIngredient>> &EndstoneRecipe::getIngredients() const
{
    if (!ingredients_built_) {
        const auto &ingredients = recipe_->getIngredients();
        ingredients_.reserve(ingredients.size());
        for (const auto &ingredient : ingredients) {
            ingredients_.push_back(EndstoneIngredient::fromMinecraft(recipe_, ingredient));
        }
        ingredients_built_ = true;
    }
    return ingredients_;
}

std::optional<endstone::RecipeIngredient> EndstoneRecipe::getSmithingIngredient(const std::size_t index) const
{
    const auto &ingredients = getIngredients();
    return index < ingredients.size() ? ingredients[index] : std::nullopt;
}

std::vector<endstone::Recipe> EndstoneRecipe::brewingRecipes()
{
    std::vector<endstone::Recipe> recipes;
    for (const auto &[plugin_id, plugin] : pluginMixes()) {
        auto input = ingredientFrom(plugin.from);
        auto reagent = ingredientFrom(plugin.reagent);
        if (!input || !reagent) {
            continue;
        }
        recipes.push_back(BrewingMixRecipe(plugin_id, std::move(*input), std::move(*reagent), stackFrom(plugin.to)));
    }
    for (const auto &[plugin_id, plugin] : pluginContainers()) {
        auto input = ingredientFrom(ItemDescriptor(*plugin.from, ItemDescriptor::ANY_AUX_VALUE));
        auto reagent = ingredientFrom(plugin.reagent);
        if (!input || !reagent) {
            continue;
        }
        recipes.push_back(
            BrewingContainerRecipe(plugin_id, std::move(*input), std::move(*reagent), stackFrom(*plugin.to)));
    }
    for (const auto &mix : PotionBrewing::getPotionMixes()) {
        const auto claimed = std::ranges::any_of(pluginMixes() | std::views::values,
                                                 [&](const PluginMix &plugin) { return sameMix(mix, plugin); });
        if (claimed) {
            continue;
        }
        auto input = ingredientFrom(mix.getFrom());
        auto reagent = ingredientFrom(mix.getIngredient());
        if (!input || !reagent) {
            continue;
        }
        recipes.push_back(BrewingMixRecipe(mixId(mix), std::move(*input), std::move(*reagent), stackFrom(mix.getTo())));
    }
    for (const auto &mix : PotionBrewing::getContainerMixes()) {
        const auto claimed =
            std::ranges::any_of(pluginContainers() | std::views::values,
                                [&](const PluginContainer &plugin) { return sameContainer(mix, plugin); });
        if (claimed) {
            continue;
        }
        auto input = ingredientFrom(ItemDescriptor(mix.getFrom(), ItemDescriptor::ANY_AUX_VALUE));
        auto reagent = ingredientFrom(mix.getIngredient());
        if (!input || !reagent) {
            continue;
        }
        recipes.push_back(
            BrewingContainerRecipe(containerId(mix), std::move(*input), std::move(*reagent), stackFrom(mix.getTo())));
    }
    return recipes;
}

bool EndstoneRecipe::registerBrewing(const Recipe &recipe)
{
    const auto &ingredients = recipe.getIngredients();
    if (ingredients.size() < 2 || !ingredients[0] || !ingredients[1]) {
        return false;
    }
    auto reagent = brewingIngredientFrom(*ingredients[1]);
    if (!reagent) {
        return false;
    }

    if (recipe.is<BrewingMixRecipe>()) {
        auto from = descriptorFrom(*ingredients[0]);
        auto to = descriptorFrom(recipe.getResult());
        if (from.getItem() == nullptr || to.getItem() == nullptr) {
            return false;
        }
        unregisterBrewing(recipe.getRecipeId());
        const auto key = mixContentKey(from, *reagent, to);
        const auto exists = std::ranges::any_of(PotionBrewing::getPotionMixes(), [&](const auto &mix) {
            return sameMix(mix, PluginMix{from, *reagent, to});
        });
        if (!exists) {
            PotionBrewing::addPotionMix(from, *reagent, to);
            pluginAddedMixKeys().insert(key);
        }
        pluginMixes().insert_or_assign(recipe.getRecipeId(), PluginMix{std::move(from), *reagent, std::move(to)});
        return true;
    }

    if (recipe.is<BrewingContainerRecipe>()) {
        auto from_desc = descriptorFrom(*ingredients[0]);
        auto to_desc = descriptorFrom(recipe.getResult());
        const auto *from = from_desc.getItem();
        const auto *to = to_desc.getItem();
        if (from == nullptr || to == nullptr) {
            return false;
        }
        unregisterBrewing(recipe.getRecipeId());
        const auto key = containerContentKey(*from, *reagent, *to);
        const auto exists = std::ranges::any_of(PotionBrewing::getContainerMixes(), [&](const auto &mix) {
            return sameContainer(mix, PluginContainer{from, *reagent, to});
        });
        if (!exists) {
            PotionBrewing::addContainerRecipe(*from, *reagent, *to);
            pluginAddedContainerKeys().insert(key);
        }
        pluginContainers().insert_or_assign(recipe.getRecipeId(), PluginContainer{from, *reagent, to});
        return true;
    }

    return false;
}

bool EndstoneRecipe::unregisterBrewing(const std::string &recipe_id)
{
    if (auto it = pluginMixes().find(recipe_id); it != pluginMixes().end()) {
        const auto plugin = it->second;
        const auto key = mixContentKey(plugin.from, plugin.reagent, plugin.to);
        pluginMixes().erase(it);
        const auto claimed = std::ranges::any_of(pluginMixes() | std::views::values, [&](const PluginMix &other) {
            return sameMixContent(plugin.from, plugin.reagent, plugin.to, other);
        });
        if (!claimed && pluginAddedMixKeys().contains(key)) {
            PotionBrewing::removePotionMix(plugin.from, plugin.reagent, plugin.to);
            pluginAddedMixKeys().erase(key);
        }
        return true;
    }
    if (auto it = pluginContainers().find(recipe_id); it != pluginContainers().end()) {
        const auto plugin = it->second;
        const auto key = containerContentKey(*plugin.from, plugin.reagent, *plugin.to);
        pluginContainers().erase(it);
        const auto claimed =
            std::ranges::any_of(pluginContainers() | std::views::values, [&](const PluginContainer &other) {
                return sameContainerContent(*plugin.from, plugin.reagent, *plugin.to, other);
            });
        if (!claimed && pluginAddedContainerKeys().contains(key)) {
            PotionBrewing::removeContainerRecipe(*plugin.from, plugin.reagent, *plugin.to);
            pluginAddedContainerKeys().erase(key);
        }
        return true;
    }
    for (const auto &mix : PotionBrewing::getPotionMixes()) {
        if (mixId(mix) == recipe_id) {
            const auto from = mix.getFrom();
            const auto reagent = mix.getIngredient();
            const auto to = mix.getTo();
            return PotionBrewing::removePotionMix(from, reagent, to);
        }
    }
    for (const auto &mix : PotionBrewing::getContainerMixes()) {
        if (containerId(mix) == recipe_id) {
            const auto &from = mix.getFrom();
            const auto reagent = mix.getIngredient();
            const auto &to = mix.getTo();
            return PotionBrewing::removeContainerRecipe(from, reagent, to);
        }
    }
    return false;
}

}  // namespace endstone::core
