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
    const auto &tag = recipe->getTag();
    if (tag == FURNACE_TAG) {
        return FurnaceRecipe(std::make_unique<EndstoneFurnaceRecipe>(std::move(recipe)));
    }
    if (tag == BLAST_FURNACE_TAG) {
        return BlastingRecipe(std::make_unique<EndstoneBlastingRecipe>(std::move(recipe)));
    }
    if (tag == SMOKER_TAG) {
        return SmokingRecipe(std::make_unique<EndstoneSmokingRecipe>(std::move(recipe)));
    }
    if (tag == CAMPFIRE_TAG || tag == SOUL_CAMPFIRE_TAG) {
        return CampfireRecipe(std::make_unique<EndstoneCampfireRecipe>(std::move(recipe)));
    }
    if (recipe->isShapeless()) {
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

std::unordered_map<std::string, PluginMix> plugin_mixes;
std::unordered_map<std::string, PluginContainer> plugin_containers;
std::unordered_set<std::string> plugin_added_mix_keys;
std::unordered_set<std::string> plugin_added_container_keys;

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
            ::Recipe::Results({ItemInstance(EndstoneItemStack::toMinecraft(transform->getResult()))}),
            HashedString(transform->getTag()));
        handle->setUnlockingRequirement(
            RecipeUnlockingRequirement(RecipeUnlockingRequirement::UnlockingContext::AlwaysUnlocked));
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
        handle->setUnlockingRequirement(
            RecipeUnlockingRequirement(RecipeUnlockingRequirement::UnlockingContext::AlwaysUnlocked));
        return handle;
    }

    if (const auto cooking = recipe.as<CookingRecipe>()) {
        ::Recipe::ConstructionContext context;
        context.recipe_id = cooking->getRecipeId();
        const auto &ingredients = cooking->getIngredients();
        context.ingredients.reserve(ingredients.size());
        for (const auto &ingredient : ingredients) {
            if (ingredient) {
                context.ingredients.push_back(EndstoneIngredient::toMinecraft(*ingredient));
            }
            else {
                context.ingredients.emplace_back();
            }
        }
        context.results = ::Recipe::Results({ItemInstance(EndstoneItemStack::toMinecraft(cooking->getResult()))});
        context.tag = HashedString(cooking->getTag());
        context.unlocking_requirement =
            RecipeUnlockingRequirement(RecipeUnlockingRequirement::UnlockingContext::AlwaysUnlocked);
        return std::make_unique<::ShapelessRecipe>(std::move(context));
    }

    if (const auto shapeless = recipe.as<ShapelessRecipe>()) {
        ::Recipe::ConstructionContext context;
        context.recipe_id = shapeless->getRecipeId();
        const auto &ingredients = shapeless->getIngredients();
        context.ingredients.reserve(ingredients.size());
        for (const auto &ingredient : ingredients) {
            if (ingredient) {
                context.ingredients.push_back(EndstoneIngredient::toMinecraft(*ingredient));
            }
            else {
                context.ingredients.emplace_back();
            }
        }
        context.results = ::Recipe::Results({ItemInstance(EndstoneItemStack::toMinecraft(shapeless->getResult()))});
        context.tag = HashedString(shapeless->getTag());
        context.unlocking_requirement =
            RecipeUnlockingRequirement(RecipeUnlockingRequirement::UnlockingContext::AlwaysUnlocked);
        return std::make_unique<::ShapelessRecipe>(std::move(context));
    }

    if (const auto shaped = recipe.as<ShapedRecipe>()) {
        ::Recipe::ConstructionContext context;
        context.recipe_id = shaped->getRecipeId();
        const auto &ingredients = shaped->getIngredients();
        context.ingredients.reserve(ingredients.size());
        for (const auto &ingredient : ingredients) {
            if (ingredient) {
                context.ingredients.push_back(EndstoneIngredient::toMinecraft(*ingredient));
            }
            else {
                context.ingredients.emplace_back();
            }
        }
        context.results = ::Recipe::Results({ItemInstance(EndstoneItemStack::toMinecraft(shaped->getResult()))});
        context.tag = HashedString(shaped->getTag());
        context.unlocking_requirement =
            RecipeUnlockingRequirement(RecipeUnlockingRequirement::UnlockingContext::AlwaysUnlocked);
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
    for (const auto &[plugin_id, plugin] : plugin_mixes) {
        auto input = EndstoneIngredient::fromMinecraft(nullptr, ::RecipeIngredient(plugin.from, 1));
        auto reagent_item =
            ItemRegistryManager::getItemRegistry().getItem(static_cast<std::int16_t>(plugin.reagent.getItemId()));
        if (reagent_item.isNull()) {
            continue;
        }
        const auto reagent_aux = plugin.reagent.getData() == PotionBrewing::Ingredient::AnyData
                                   ? ItemDescriptor::ANY_AUX_VALUE
                                   : plugin.reagent.getData();
        auto reagent = EndstoneIngredient::fromMinecraft(nullptr, ::RecipeIngredient(*reagent_item, reagent_aux, 1));
        if (!input || !reagent) {
            continue;
        }
        const auto *output = plugin.to.getItem();
        const auto result = output == nullptr
                              ? endstone::ItemType::get(endstone::ItemType::Air)->createItemStack(0)
                              : EndstoneItemStack::fromMinecraft(::ItemStack(*output, 1, plugin.to.getAuxValue()));
        recipes.push_back(BrewingMixRecipe(plugin_id, std::move(*input), std::move(*reagent), result));
    }
    for (const auto &[plugin_id, plugin] : plugin_containers) {
        auto input = EndstoneIngredient::fromMinecraft(
            nullptr, ::RecipeIngredient(ItemDescriptor(*plugin.from, ItemDescriptor::ANY_AUX_VALUE), 1));
        auto reagent_item =
            ItemRegistryManager::getItemRegistry().getItem(static_cast<std::int16_t>(plugin.reagent.getItemId()));
        if (reagent_item.isNull()) {
            continue;
        }
        const auto reagent_aux = plugin.reagent.getData() == PotionBrewing::Ingredient::AnyData
                                   ? ItemDescriptor::ANY_AUX_VALUE
                                   : plugin.reagent.getData();
        auto reagent = EndstoneIngredient::fromMinecraft(nullptr, ::RecipeIngredient(*reagent_item, reagent_aux, 1));
        if (!input || !reagent) {
            continue;
        }
        recipes.push_back(BrewingContainerRecipe(plugin_id, std::move(*input), std::move(*reagent),
                                                 EndstoneItemStack::fromMinecraft(::ItemStack(*plugin.to, 1))));
    }
    for (const auto &mix : PotionBrewing::getPotionMixes()) {
        const auto claimed = std::ranges::any_of(plugin_mixes | std::views::values, [&](const PluginMix &plugin) {
            return mix.getFrom().sameItem(plugin.from, true) && mix.getIngredient() == plugin.reagent &&
                   mix.getTo().sameItem(plugin.to, true);
        });
        if (claimed) {
            continue;
        }
        auto input = EndstoneIngredient::fromMinecraft(nullptr, ::RecipeIngredient(mix.getFrom(), 1));
        auto reagent_item =
            ItemRegistryManager::getItemRegistry().getItem(static_cast<std::int16_t>(mix.getIngredient().getItemId()));
        if (reagent_item.isNull()) {
            continue;
        }
        const auto reagent_aux = mix.getIngredient().getData() == PotionBrewing::Ingredient::AnyData
                                   ? ItemDescriptor::ANY_AUX_VALUE
                                   : mix.getIngredient().getData();
        auto reagent = EndstoneIngredient::fromMinecraft(nullptr, ::RecipeIngredient(*reagent_item, reagent_aux, 1));
        if (!input || !reagent) {
            continue;
        }
        const auto *output = mix.getTo().getItem();
        const auto result = output == nullptr
                              ? endstone::ItemType::get(endstone::ItemType::Air)->createItemStack(0)
                              : EndstoneItemStack::fromMinecraft(::ItemStack(*output, 1, mix.getTo().getAuxValue()));
        recipes.push_back(BrewingMixRecipe(std::format("minecraft:brewing_mix/{}:{}+{}:{}/{}:{}",
                                                       mix.getFrom().getFullName(), mix.getFrom().getAuxValue(),
                                                       mix.getIngredient().getItemId(), mix.getIngredient().getData(),
                                                       mix.getTo().getFullName(), mix.getTo().getAuxValue()),
                                           std::move(*input), std::move(*reagent), result));
    }
    for (const auto &mix : PotionBrewing::getContainerMixes()) {
        const auto claimed =
            std::ranges::any_of(plugin_containers | std::views::values, [&](const PluginContainer &plugin) {
                return mix.getFrom().getId() == plugin.from->getId() && mix.getIngredient() == plugin.reagent &&
                       mix.getTo().getId() == plugin.to->getId();
            });
        if (claimed) {
            continue;
        }
        auto input = EndstoneIngredient::fromMinecraft(
            nullptr, ::RecipeIngredient(ItemDescriptor(mix.getFrom(), ItemDescriptor::ANY_AUX_VALUE), 1));
        auto reagent_item =
            ItemRegistryManager::getItemRegistry().getItem(static_cast<std::int16_t>(mix.getIngredient().getItemId()));
        if (reagent_item.isNull()) {
            continue;
        }
        const auto reagent_aux = mix.getIngredient().getData() == PotionBrewing::Ingredient::AnyData
                                   ? ItemDescriptor::ANY_AUX_VALUE
                                   : mix.getIngredient().getData();
        auto reagent = EndstoneIngredient::fromMinecraft(nullptr, ::RecipeIngredient(*reagent_item, reagent_aux, 1));
        if (!input || !reagent) {
            continue;
        }
        recipes.push_back(BrewingContainerRecipe(
            std::format("minecraft:brewing_container/{}/{}:{}/{}", mix.getFrom().getFullItemName(),
                        mix.getIngredient().getItemId(), mix.getIngredient().getData(), mix.getTo().getFullItemName()),
            std::move(*input), std::move(*reagent), EndstoneItemStack::fromMinecraft(::ItemStack(mix.getTo(), 1))));
    }
    return recipes;
}

bool EndstoneRecipe::registerBrewing(const Recipe &recipe)
{
    const auto &ingredients = recipe.getIngredients();
    if (ingredients.size() < 2 || !ingredients[0] || !ingredients[1]) {
        return false;
    }

    const auto reagent_minecraft = EndstoneIngredient::toMinecraft(*ingredients[1]);
    const auto *reagent_item = reagent_minecraft.getItem();
    if (reagent_item == nullptr) {
        return false;
    }
    const auto reagent_aux = reagent_minecraft.getAuxValue();
    const PotionBrewing::Ingredient reagent(
        *reagent_item, reagent_aux == ItemDescriptor::ANY_AUX_VALUE ? PotionBrewing::Ingredient::AnyData : reagent_aux);

    if (recipe.is<BrewingMixRecipe>()) {
        const auto from = EndstoneIngredient::toMinecraft(*ingredients[0]);
        const auto result_minecraft = EndstoneItemStack::toMinecraft(recipe.getResult());
        const auto *result_item = result_minecraft.getItem();
        if (from.getItem() == nullptr || result_item == nullptr) {
            return false;
        }
        const ItemDescriptor to(*result_item, result_minecraft.getAuxValue());

        unregisterBrewing(recipe.getRecipeId());
        const auto key = std::format("{}:{}+{}:{}/{}:{}", from.getFullName(), from.getAuxValue(), reagent.getItemId(),
                                     reagent.getData(), to.getFullName(), to.getAuxValue());
        const auto exists = std::ranges::any_of(PotionBrewing::getPotionMixes(), [&](const auto &mix) {
            return mix.getFrom().sameItem(from, true) && mix.getIngredient() == reagent &&
                   mix.getTo().sameItem(to, true);
        });
        if (!exists) {
            PotionBrewing::addPotionMix(from, reagent, to);
            plugin_added_mix_keys.insert(key);
        }
        plugin_mixes.insert_or_assign(recipe.getRecipeId(), PluginMix{from, reagent, to});
        return true;
    }

    if (recipe.is<BrewingContainerRecipe>()) {
        const auto from_descriptor = EndstoneIngredient::toMinecraft(*ingredients[0]);
        const auto result_minecraft = EndstoneItemStack::toMinecraft(recipe.getResult());
        const auto *from = from_descriptor.getItem();
        const auto *to = result_minecraft.getItem();
        if (from == nullptr || to == nullptr) {
            return false;
        }

        unregisterBrewing(recipe.getRecipeId());
        const auto key = std::format("{}/{}:{}/{}", from->getFullItemName(), reagent.getItemId(), reagent.getData(),
                                     to->getFullItemName());
        const auto exists = std::ranges::any_of(PotionBrewing::getContainerMixes(), [&](const auto &mix) {
            return mix.getFrom().getId() == from->getId() && mix.getIngredient() == reagent &&
                   mix.getTo().getId() == to->getId();
        });
        if (!exists) {
            PotionBrewing::addContainerRecipe(*from, reagent, *to);
            plugin_added_container_keys.insert(key);
        }
        plugin_containers.insert_or_assign(recipe.getRecipeId(), PluginContainer{from, reagent, to});
        return true;
    }

    return false;
}

bool EndstoneRecipe::unregisterBrewing(const std::string &recipe_id)
{
    if (auto it = plugin_mixes.find(recipe_id); it != plugin_mixes.end()) {
        const auto plugin = it->second;
        const auto key = std::format("{}:{}+{}:{}/{}:{}", plugin.from.getFullName(), plugin.from.getAuxValue(),
                                     plugin.reagent.getItemId(), plugin.reagent.getData(), plugin.to.getFullName(),
                                     plugin.to.getAuxValue());
        plugin_mixes.erase(it);
        const auto claimed = std::ranges::any_of(plugin_mixes | std::views::values, [&](const PluginMix &other) {
            return plugin.from.sameItem(other.from, true) && plugin.reagent == other.reagent &&
                   plugin.to.sameItem(other.to, true);
        });
        if (!claimed && plugin_added_mix_keys.contains(key)) {
            PotionBrewing::removePotionMix(plugin.from, plugin.reagent, plugin.to);
            plugin_added_mix_keys.erase(key);
        }
        return true;
    }
    if (auto it = plugin_containers.find(recipe_id); it != plugin_containers.end()) {
        const auto plugin = it->second;
        const auto key = std::format("{}/{}:{}/{}", plugin.from->getFullItemName(), plugin.reagent.getItemId(),
                                     plugin.reagent.getData(), plugin.to->getFullItemName());
        plugin_containers.erase(it);
        const auto claimed =
            std::ranges::any_of(plugin_containers | std::views::values, [&](const PluginContainer &other) {
                return plugin.from->getId() == other.from->getId() && plugin.reagent == other.reagent &&
                       plugin.to->getId() == other.to->getId();
            });
        if (!claimed && plugin_added_container_keys.contains(key)) {
            PotionBrewing::removeContainerRecipe(*plugin.from, plugin.reagent, *plugin.to);
            plugin_added_container_keys.erase(key);
        }
        return true;
    }
    for (const auto &mix : PotionBrewing::getPotionMixes()) {
        if (std::format("minecraft:brewing_mix/{}:{}+{}:{}/{}:{}", mix.getFrom().getFullName(),
                        mix.getFrom().getAuxValue(), mix.getIngredient().getItemId(), mix.getIngredient().getData(),
                        mix.getTo().getFullName(), mix.getTo().getAuxValue()) == recipe_id) {
            const auto from = mix.getFrom();
            const auto reagent = mix.getIngredient();
            const auto to = mix.getTo();
            return PotionBrewing::removePotionMix(from, reagent, to);
        }
    }
    for (const auto &mix : PotionBrewing::getContainerMixes()) {
        if (std::format("minecraft:brewing_container/{}/{}:{}/{}", mix.getFrom().getFullItemName(),
                        mix.getIngredient().getItemId(), mix.getIngredient().getData(),
                        mix.getTo().getFullItemName()) == recipe_id) {
            const auto &from = mix.getFrom();
            const auto reagent = mix.getIngredient();
            const auto &to = mix.getTo();
            return PotionBrewing::removeContainerRecipe(from, reagent, to);
        }
    }
    return false;
}

}  // namespace endstone::core
