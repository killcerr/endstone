#pragma once
#include <memory>

#include "endstone/namespaced_key.h"
#include "endstone/potion/potion_effect_type_category.h"

namespace endstone {
class PotionEffect;
class PotionEffectType {
public:
    virtual ~PotionEffectType() = 0;
    [[nodiscard]] virtual std::shared_ptr<PotionEffect> createEffect(int duration, int amplifier) = 0;
    [[nodiscard]] virtual bool isInstant() = 0;
    [[nodiscard]] virtual PotionEffectTypeCategory getCategory() = 0;
    [[nodiscard]] virtual std::string getTranslationKey() const = 0;
    [[nodiscard]] virtual std::string getName();
    [[nodiscard]] virtual NamespacedKey getKey();
};
}  // namespace endstone
