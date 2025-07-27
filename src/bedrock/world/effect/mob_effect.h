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
#include <vector>

#include "bedrock/core/math/color.h"
#include "bedrock/core/string/string_hash.h"
#include "bedrock/shared_types/legacy/level_sound_event.h"
#include "bedrock/world/attribute/amplifier.h"
#include "bedrock/world/attribute/attribute.h"
#include "bedrock/world/attribute/attribute_buff.h"
#include "bedrock/world/attribute/attribute_modifier.h"
#include "bedrock/world/effect/effect_duration.h"
class MobEffect {
public:
    static constexpr std::size_t NUM_EFFECTS = 37;
    static constexpr int UNKNOWN_EFFECT = 0;
    static constexpr int DARKNESS_EFFECT_FACTOR_PADDING_DURATION = 22;
    struct FactorCalculationData {
        int padding_duration;
        float factor_start;
        float factor_target;
        float factor_current;
        std::function<void(FactorCalculationData &, EffectDuration)> update_fn;
        int ticks_active;
        float factor_previous_frame;
        bool had_effect_last_tick;
        bool had_applied;
    };
    unsigned const id;
    bool is_harmful;
    mce::Color color;
    HashedString particle_effect_id;
    HashedString particle_effect_ambient_id;
    std::string description_id;
    int icon;
    float duration_modifier;
    bool is_disabled;
    std::string resource_name;
    std::string icon_name;
    bool effect_visible;
    std::optional<::SharedTypes::Legacy::LevelSoundEvent> on_apply_sound;
    HashedString component_name;
    std::shared_ptr<Amplifier> value_amplifier;
    std::shared_ptr<Amplifier> duration_amplifier;
    std::vector<std::pair<Attribute const *, std::shared_ptr<AttributeBuff>>> attribute_buffs;
    std::vector<std::pair<Attribute const *, std::shared_ptr<AttributeModifier>>> attribute_modifiers;
    FactorCalculationData factor_calculation_data;
    static std::add_lvalue_reference_t<std::unique_ptr<MobEffect>[]> getEffects();
};
