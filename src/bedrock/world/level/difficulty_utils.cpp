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

#include "bedrock/world/level/difficulty_utils.h"

#include <algorithm>

float DifficultyUtils::getSpecialMultiplier(Difficulty difficulty, int time, float moon_brightness)
{
    if (difficulty == Difficulty::Peaceful) {
        return 0.0F;
    }

    float daytime = static_cast<float>(time) - 0.5F;
    float raw_factor = std::max(0.0F, daytime) * 0.25F;
    float daytime_factor = (1.0F < daytime) ? 0.25F : raw_factor;

    float moon_factor = std::max(0.0F, moon_brightness * 0.25F);
    float time_factor = std::min(daytime_factor, moon_factor);

    float v11 = time_factor;
    if (difficulty == Difficulty::Hard) {
        v11 += 0.375F;
    }

    if (difficulty == Difficulty::Easy) {
        v11 *= 0.5F;
    }

    float value = static_cast<float>(difficulty) * (daytime_factor + 0.75F + v11);

    if (value < 2.0F) {
        return 0.0F;
    }
    if (value > 4.0F) {
        return 1.0F;
    }
    return (value - 2.0F) * 0.5F;
}
