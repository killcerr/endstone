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

#include <cstddef>
#include <functional>
#include <vector>

#include "bedrock/bedrock.h"
#include "bedrock/world/item/item.h"
#include "bedrock/world/item/item_descriptor.h"
#include "bedrock/world/item/item_instance.h"

class PotionBrewing {
public:
    class Ingredient {
    public:
        Ingredient(const Item &item, int data);
        explicit Ingredient(const ItemInstance &item);
        bool operator==(const Ingredient &other) const;
        [[nodiscard]] bool equals(const ItemDescriptor &input) const;
        [[nodiscard]] int getItemId() const { return item_id_; }
        [[nodiscard]] int getData() const { return data_; }

        static constexpr int AnyData = -1;

    private:
        int item_id_;
        int data_;
    };

    template <typename T>
    class Mix;

    template <>
    class Mix<ItemDescriptor> {
    public:
        Mix(ItemDescriptor from, const Ingredient &ingredient, ItemDescriptor to)
            : from_(std::move(from)), ingredient_(ingredient), to_(std::move(to))
        {
        }

        [[nodiscard]] const ItemDescriptor &getFrom() const { return from_; }
        [[nodiscard]] const ItemDescriptor &getTo() const { return to_; }
        [[nodiscard]] const Ingredient &getIngredient() const { return ingredient_; }

    private:
        ItemDescriptor from_;
        Ingredient ingredient_;
        ItemDescriptor to_;
    };

    template <>
    class Mix<const Item &> {
    public:
        Mix(const Item &from, const Ingredient &ingredient, const Item &to)
            : from_(from), ingredient_(ingredient), to_(to)
        {
        }

        [[nodiscard]] const Item &getFrom() const { return from_; }
        [[nodiscard]] const Item &getTo() const { return to_; }
        [[nodiscard]] const Ingredient &getIngredient() const { return ingredient_; }

    private:
        const Item &from_;
        Ingredient ingredient_;
        const Item &to_;
    };

    template <>
    class Mix<ItemInstance> {
    public:
        Mix(ItemInstance from, const Ingredient &ingredient, ItemInstance to)
            : from_(std::move(from)), ingredient_(ingredient), to_(std::move(to))
        {
        }

        [[nodiscard]] const ItemInstance &getFrom() const { return from_; }
        [[nodiscard]] const ItemInstance &getTo() const { return to_; }
        [[nodiscard]] const Ingredient &getIngredient() const { return ingredient_; }

    private:
        ItemInstance from_;
        Ingredient ingredient_;
        ItemInstance to_;
    };

    static void addPotionMix(const ItemDescriptor &from, const Ingredient &ingredient, const ItemDescriptor &to);
    static void addContainerRecipe(const Item &from, const Ingredient &ingredient, const Item &to);
    static bool removePotionMix(const ItemDescriptor &from, const Ingredient &ingredient, const ItemDescriptor &to);
    static bool removeContainerRecipe(const Item &from, const Ingredient &ingredient, const Item &to);

    static const std::vector<Mix<ItemDescriptor>> &getPotionMixes();
    static const std::vector<Mix<const Item &>> &getContainerMixes();
};

namespace std {
template <>
struct hash<PotionBrewing::Ingredient> {
    std::size_t operator()(const PotionBrewing::Ingredient &value) const noexcept
    {
        static std::hash<int> hasher;
        std::size_t seed = 0;
        seed ^= hasher(value.getItemId()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hasher(value.getData()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};
}  // namespace std

BEDROCK_STATIC_ASSERT_SIZE(PotionBrewing::Ingredient, 8, 8);
BEDROCK_STATIC_ASSERT_SIZE(PotionBrewing::Mix<ItemDescriptor>, 40, 40);
BEDROCK_STATIC_ASSERT_SIZE(PotionBrewing::Mix<const Item &>, 24, 24);
BEDROCK_STATIC_ASSERT_SIZE(PotionBrewing::Mix<ItemInstance>, 264, 264);
