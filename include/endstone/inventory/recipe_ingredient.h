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

#include <optional>
#include <string>
#include <utility>

namespace endstone {

/** Identifies the descriptor represented by a recipe ingredient. */
enum class RecipeIngredientKind {
    Empty,
    Item,
    ItemTag,
    Unsupported,
};

/** Describes one immutable ingredient captured from a server recipe. */
class RecipeIngredient {
public:
    using Kind = RecipeIngredientKind;
    static constexpr int AnyData = 0x7fff;

    RecipeIngredient() = default;
    RecipeIngredient(Kind kind, std::string identifier, int count = 1,
                     std::optional<int> data = std::nullopt)
        : kind_(kind), identifier_(std::move(identifier)), count_(count), data_(data)
    {
    }

    /** Gets the descriptor kind. */
    [[nodiscard]] Kind getKind() const noexcept { return kind_; }
    /** Gets the item identifier, item tag, or opaque descriptor name. */
    [[nodiscard]] const std::string &getIdentifier() const noexcept { return identifier_; }
    [[nodiscard]] const std::string &getItemId() const noexcept
    {
        static const std::string empty;
        return kind_ == Kind::Item ? identifier_ : empty;
    }
    /** Gets the item tag, or an empty string for another kind. */
    [[nodiscard]] const std::string &getTag() const noexcept
    {
        static const std::string empty;
        return kind_ == Kind::ItemTag ? identifier_ : empty;
    }
    /** Gets the required ingredient count. */
    [[nodiscard]] int getCount() const noexcept { return count_; }
    /** Gets the exact auxiliary data constraint, or no value for a wildcard. */
    [[nodiscard]] std::optional<int> getData() const noexcept { return data_; }
    /** Gets the auxiliary data constraint using Bedrock's wildcard sentinel. */
    [[nodiscard]] int getAuxValue() const noexcept { return data_.value_or(AnyData); }
    /** Checks whether this ingredient occupies an empty shaped-recipe slot. */
    [[nodiscard]] bool isEmpty() const noexcept { return kind_ == Kind::Empty; }
    /** Checks whether this ingredient matches an item tag. */
    [[nodiscard]] bool isItemTag() const noexcept { return kind_ == Kind::ItemTag; }
    /** Checks whether any auxiliary data is accepted. */
    [[nodiscard]] bool isWildcardData() const noexcept { return !data_.has_value(); }
    /** Checks whether the descriptor kind is supported. */
    [[nodiscard]] bool isSupported() const noexcept { return kind_ != Kind::Unsupported; }

private:
    Kind kind_{Kind::Empty};
    std::string identifier_;
    int count_{0};
    std::optional<int> data_;
};

}  // namespace endstone
