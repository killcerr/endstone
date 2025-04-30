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

class Enchant {
public:
    enum class Type : unsigned char {
        Protection = 0,
        FireProtection = 1,
        FeatherFalling = 2,
        BlastProtection = 3,
        ProjectileProtection = 4,
        Thorns = 5,
        Respiration = 6,
        DepthStrider = 7,
        AquaAffinity = 8,
        Sharpness = 9,
        Smite = 10,
        BaneOfArthropods = 11,
        Knockback = 12,
        FireAspect = 13,
        Looting = 14,
        Efficiency = 15,
        SilkTouch = 16,
        Unbreaking = 17,
        Fortune = 18,
        Power = 19,
        Punch = 20,
        Flame = 21,
        Infinity = 22,
        LuckOfTheSea = 23,
        Lure = 24,
        FrostWalker = 25,
        Mending = 26,
        CurseOfBinding = 27,
        CurseOfVanishing = 28,
        Impaling = 29,
        Riptide = 30,
        Loyalty = 31,
        Channeling = 32,
        Multishot = 33,
        Piercing = 34,
        QuickCharge = 35,
        SoulSpeed = 36,
        SwiftSneak = 37,
        WindBurst = 38,
        Density = 39,
        Breach = 40,
        NumEnchantments = 41,
        InvalidEnchantment = 42,
    };
};
