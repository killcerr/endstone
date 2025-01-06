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

#include "bedrock/server/commands/command.h"

#include "endstone/runtime/symbol.h"

RelativeFloat::RelativeFloat(float offset, bool relative) : offset_(offset), relative_(relative) {}

float RelativeFloat::getValue(float base) const
{
    return relative_ ? base + offset_ : offset_;
}

bool RelativeFloat::isRelative() const
{
    return relative_;
}

std::string Command::getCommandName() const
{
    ENDSTONE_SYMCALL(&Command::getCommandName, this);
}

void Command::run(CommandOrigin const &origin, CommandOutput &output) const
{
    ENDSTONE_SYMCALL(&Command::run, this, origin, output);
}

bool Command::collectOptionalArguments()
{
    return true;
}