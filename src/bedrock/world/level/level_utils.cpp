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

#include "bedrock/world/level/level_utils.h"

#include "bedrock/world/level/level_interface.h"

int LevelUtils::getDay(int time)
{
    return time / 24000;
}

int LevelUtils::getTimeOfDay(int time)
{
    int time_of_day = time % 24000;
    if (time_of_day < 0) {
        time_of_day += 24000;
    }
    return time_of_day;
}

void LevelUtils::getDayAndTimeOfDay(int time, int &day, int &time_of_day)
{
    day = time / 24000;
    time_of_day = time % 24000;
    if (time_of_day < 0) {
        --day;
        time_of_day += 24000;
    }
}

int LevelUtils::projectToTimeOfDay(int seed, int time)
{
    int result = time % 24000;
    if (result < 0) {
        result += 24000;
    }
    return result;
}

int LevelUtils::getMoonPhase(const ILevel &level)
{
    int time = level.getTime();
    int days = time / 24000;
    return days & 7;
}
