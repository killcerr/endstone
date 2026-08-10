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

#include "endstone/block/container.h"

namespace endstone {

/**
 * Represents a captured state of a furnace.
 */
class Furnace : public Container {
public:
    /**
     * Gets the burn time.
     *
     * <p>
     * This is the number of ticks the current fuel keeps the furnace lit for.
     *
     * @return the burn time
     */
    [[nodiscard]] virtual int getBurnTime() const = 0;

    /**
     * Sets the burn time.
     *
     * <p>
     * A burn time greater than 0 will cause this block to be lit, whilst a time less than 0 will extinguish it.
     *
     * @param burn_time the burn time
     */
    virtual void setBurnTime(int burn_time) = 0;

    /**
     * Gets the cook time.
     *
     * <p>
     * This is the amount of time the item has been cooking for.
     *
     * @return the cook time
     */
    [[nodiscard]] virtual int getCookTime() const = 0;

    /**
     * Sets the cook time.
     *
     * <p>
     * This is the amount of time the item has been cooking for.
     *
     * @param cook_time the cook time
     */
    virtual void setCookTime(int cook_time) = 0;
};

}  // namespace endstone
