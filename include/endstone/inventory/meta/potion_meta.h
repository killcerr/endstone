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

#include "endstone/inventory/meta/item_meta.h"
#include "endstone/potion/potion_type.h"

namespace endstone {
/**
 * Represents the metadata for a potion item.
 *
 * A potion carries a single base potion type, which determines both the effects it applies and the colour it is
 * rendered with.
 */
class PotionMeta : public ItemMeta {
public:
    /**
     * Checks for the presence of a base potion type.
     *
     * @return true if this has a base potion type
     */
    [[nodiscard]] virtual bool hasBasePotionType() const = 0;

    /**
     * Gets the base potion type of this potion.
     *
     * @return the base potion type, or <code>std::nullopt</code> if none is set
     */
    [[nodiscard]] virtual std::optional<PotionId> getBasePotionType() const = 0;

    /**
     * Sets the base potion type of this potion, or removes it when given <code>std::nullopt</code>.
     *
     * @param type the base potion type to set
     */
    virtual void setBasePotionType(std::optional<PotionId> type) = 0;
};
}  // namespace endstone
