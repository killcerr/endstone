#include "bedrock/world/effect/mob_effect.h"

#include "bedrock/symbol.h"

std::add_lvalue_reference_t<std::unique_ptr<MobEffect>[]> MobEffect::getEffects()
{
    return BEDROCK_VAR(std::add_lvalue_reference_t<std::unique_ptr<MobEffect>[]>, "MobEffect::mMobEffects");
}
