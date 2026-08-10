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

namespace endstone {

/**
 * Represents a rotation based orientation, like that on a clock.
 *
 * <p>
 * It represents how something is viewed, as opposed to cardinal directions.
 */
enum class Rotation {
    /**
     * No rotation.
     */
    None = 0,

    /**
     * Rotated clockwise by 45 degrees.
     */
    Clockwise45 = 1,

    /**
     * Rotated clockwise by 90 degrees.
     */
    Clockwise = 2,

    /**
     * Rotated clockwise by 135 degrees.
     */
    Clockwise135 = 3,

    /**
     * Flipped upside-down, a 180 degree rotation.
     */
    Flipped = 4,

    /**
     * Flipped upside-down + 45 degree rotation.
     */
    Flipped45 = 5,

    /**
     * Rotated counter-clockwise by 90 degrees.
     */
    CounterClockwise = 6,

    /**
     * Rotated counter-clockwise by 45 degrees.
     */
    CounterClockwise45 = 7,
};

}  // namespace endstone
