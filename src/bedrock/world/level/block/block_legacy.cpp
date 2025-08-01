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

#include "bedrock/world/level/block/block_legacy.h"

#include "bedrock/symbol.h"
#include "bedrock/world/item/registry/item_registry.h"

bool BlockLegacy::hasProperty(BlockProperty property) const
{
    return (static_cast<std::underlying_type_t<BlockProperty>>(property) &
            static_cast<std::underlying_type_t<BlockProperty>>(properties_)) != 0;
}

const Block *BlockLegacy::tryGetStateFromLegacyData(DataID data) const
{
    return BEDROCK_CALL(&BlockLegacy::tryGetStateFromLegacyData, this, data);
}

bool BlockLegacy::hasState(const BlockState &block_state) const
{
    if (states_.contains(block_state.getID())) {
        return true;
    }
    for (const auto &altered_state : altered_state_collections_) {
        if (altered_state->getBlockState().getID() == block_state.getID()) {
            return true;
        }
    }
    return false;
}

bool BlockLegacy::hasState(const HashedString &name) const
{
    auto *state = BlockStateRegistry::get().getState(name);
    if (!state) {
        return false;
    }
    return hasState(*state);
}

bool BlockLegacy::requiresCorrectToolForDrops() const
{
    return requires_correct_tool_for_drops_;
}

bool BlockLegacy::isSolid() const
{
    return solid_;
}

float BlockLegacy::getThickness() const
{
    return thickness_;
}

float BlockLegacy::getTranslucency() const
{
    return translucency_;
}

const std::vector<HashedString> &BlockLegacy::getTags() const
{
    return tags_;
}

const Material &BlockLegacy::getMaterial() const
{
    return material_;
}

const std::string &BlockLegacy::getDescriptionId() const
{
    return description_id;
}

const std::string &BlockLegacy::getRawNameId() const
{
    return name_info_.raw_name.getString();
}

const std::string &BlockLegacy::getNamespace() const
{
    return name_info_.namespace_name;
}

const HashedString &BlockLegacy::getName() const
{
    return name_info_.full_name;
}

bool BlockLegacy::anyOf(const gsl::span<const std::reference_wrapper<const HashedString>> &block_type_ids) const
{
    return std::ranges::any_of(block_type_ids,
                               [this](const auto &block_type_id) { return block_type_id.get() == getName(); });
}

const Block &BlockLegacy::getDefaultState() const
{
    return *default_state_;
}

const BaseGameVersion &BlockLegacy::getRequiredBaseGameVersion() const
{
    return min_required_game_version_;
}

std::int16_t BlockLegacy::getBlockItemId() const
{
    const auto value = id_.value;
    if (value < ItemRegistry::START_ITEM_ID) {
        return static_cast<std::int16_t>(value);
    }
    return static_cast<std::int16_t>((ItemRegistry::START_ITEM_ID - 1u) - value);
}

TintMethod BlockLegacy::getTintMethod() const
{
    return tint_method_;
}

void BlockLegacy::forEachBlockPermutation(std::function<bool(Block const &)> callback) const
{
    for (const auto &block_permutation : block_permutations_) {
        if (block_permutation) {
            (void)callback(*block_permutation);
        }
    }
}

std::optional<int> BlockLegacy::_tryLookupAlteredStateCollection(size_t id, DataID data) const
{
    if (altered_state_collections_.empty()) {
        return std::nullopt;
    }
    for (const auto &altered_state : altered_state_collections_) {
        if (altered_state->getBlockState().getID() == id) {
            return altered_state->getState(*this, data);
        }
    }
    return std::nullopt;
}

const Block *BlockLegacy::_trySetStateFromAlteredStateCollection(size_t id, int val, DataID data) const
{
    if (altered_state_collections_.empty()) {
        return nullptr;
    }
    for (const auto &altered_state : altered_state_collections_) {
        if (altered_state->getBlockState().getID() == id) {
            return altered_state->setState(*this, data, val);
        }
    }
    return nullptr;
}
