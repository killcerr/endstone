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

#include "bedrock/resources/resource_pack_manager.h"
#include "bedrock/world/item/crafting/recipe.h"
class Level;
class Recipes {
public:
    struct FurnaceRecipeKey {
        int id;
        HashedString tag;
    };
    ResourcePackManager *manager;
    char pad1[8];
    std::map<HashedString, std::map<std::string, std::shared_ptr<Recipe>>> recipes;
    std::map<::Recipes::FurnaceRecipeKey, ::ItemInstance> furnace_recipes;
    bool initializing;
    char pad2[16];
    std::unordered_map<::RecipeNetId, ::Recipe *> recipes_by_net_id;
    char pad3[112];
    Level *level;
};
