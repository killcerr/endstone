#pragma once
#include <memory>
#include <string>

namespace endstone {
class PotionEffectType;
class Actor;
class PotionEffect {
public:
    static constexpr int INFINITE_DURATION = -1;  // todo: check mcbe value
    virtual ~PotionEffect() = 0;
    [[nodiscard]] virtual int getAmplifier() = 0;
    [[nodiscard]] virtual int getDuration() const = 0;
    [[nodiscard]] virtual bool isShorterThan(const PotionEffect &other) const = 0;
    [[nodiscard]] virtual bool isInfinite() const = 0;
    [[nodiscard]] virtual bool apply(Actor &actor) const = 0;
    [[nodiscard]] virtual std::string toString() = 0;
    [[nodiscard]] virtual bool hasParticles() = 0;
    [[nodiscard]] virtual std::shared_ptr<PotionEffectType> getType() = 0;
};

}  // namespace endstone
