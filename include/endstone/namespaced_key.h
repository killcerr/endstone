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

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>

#include "endstone/plugin/plugin.h"
#include "endstone/util/result.h"

namespace endstone {

/**
 * @brief Represents a string-based key which consists of two components - a namespace and a key.
 *
 * Namespaces may only contain lowercase alphanumeric characters, periods, underscores, and hyphens.
 *
 * Keys may only contain lowercase alphanumeric characters, periods, underscores, hyphens, and forward slashes.
 */
class NamespacedKey final {
public:
    /**
     * The namespace representing all inbuilt keys.
     */
    static constexpr std::string_view MINECRAFT = "minecraft";

    /**
     * @brief Create a key in the plugin's namespace.
     *
     * @param plugin the plugin to use for the namespace
     * @param key the key to create
     */
    static Result<NamespacedKey> create(const Plugin &plugin, std::string_view key)
    {
        std::string ns = normalizeString(plugin.getName());
        std::string k = normalizeString(key);

        ENDSTONE_CHECKF(isValidNamespace(ns), "Invalid namespace. Must be [a-z0-9._-]: {}", ns);
        ENDSTONE_CHECKF(isValidKey(k), "Invalid key. Must be [a-z0-9/._-]: {}", k);
        return NamespacedKey(std::move(ns), std::move(k));
    }

    /**
     * Parse a NamespacedKey from a string, with an optional default namespace.
     *
     * Examples:
     * - fromString("foo", plugin)      -> "plugin:foo"
     * - fromString("foo:bar", nullptr) -> "foo:bar"
     * - fromString(":foo", nullptr)    -> "minecraft:foo"
     * - fromString("foo", nullptr)     -> "minecraft:foo"
     * - fromString("Foo", plugin)      -> std::nullopt
     *
     * @param input the string to parse
     * @param plugin optional default namespace; if nullptr, uses the minecraft namespace
     * @return optional containing the parsed key, or std::nullopt if invalid
     */
    static Result<NamespacedKey> fromString(std::string_view input, const Plugin *plugin = nullptr)
    {
        ENDSTONE_CHECK(!input.empty(), "Input string must not be empty or null");

        auto colon_count = static_cast<size_t>(std::count(input.begin(), input.end(), ':'));
        ENDSTONE_CHECK(colon_count <= 1, "Input string is invalid: " + std::string(input));

        std::string_view ns;
        std::string_view k;
        if (colon_count == 1) {
            const auto pos = input.find(':');
            ns = input.substr(0, pos);
            k = input.substr(pos + 1);
        }
        else {
            k = input;
        }

        ENDSTONE_CHECK(isValidKey(k), "Invalid key. Must be [a-z0-9/._-]: " + std::string(k));

        if (colon_count == 0 || ns.empty()) {
            return plugin ? create(*plugin, k) : NamespacedKey(MINECRAFT, k);
        }

        ENDSTONE_CHECK(isValidNamespace(ns), "Invalid namespace. Must be [a-z0-9._-]: " + std::string(ns));
        return NamespacedKey(ns, k);
    }

    [[nodiscard]] const std::string &getNamespace() const noexcept
    {
        return namespace_;
    }
    [[nodiscard]] const std::string &getKey() const noexcept
    {
        return key_;
    }

    [[nodiscard]] std::string toString() const noexcept
    {
        return namespace_ + ':' + key_;
    }

    bool operator==(const NamespacedKey &other) const noexcept
    {
        return namespace_ == other.namespace_ && key_ == other.key_;
    }

    bool operator!=(const NamespacedKey &other) const noexcept
    {
        return !(*this == other);
    }

private:
    std::string namespace_;
    std::string key_;

    /**
     * Private constructor; use create() or fromString() instead.
     */
    NamespacedKey(const std::string_view ns, const std::string_view key) : namespace_(ns), key_(key) {}

    /**
     * Normalize a string view to lowercase, returning a new std::string.
     */
    static std::string normalizeString(const std::string_view str)
    {
        std::string s(str);
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

    static constexpr bool isValidNamespaceChar(char c) noexcept
    {
        return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-';
    }

    static constexpr bool isValidKeyChar(char c) noexcept
    {
        return isValidNamespaceChar(c) || c == '/';
    }

    static bool isValidNamespace(const std::string_view ns) noexcept
    {
        if (ns.empty()) {
            return false;
        }
        return std::all_of(ns.begin(), ns.end(), isValidNamespaceChar);
    }

    static bool isValidKey(const std::string_view key) noexcept
    {
        if (key.empty()) {
            return false;
        }
        return std::all_of(key.begin(), key.end(), isValidKeyChar);
    }
};

}  // namespace endstone

template <>
struct std::hash<endstone::NamespacedKey> {
    size_t operator()(const endstone::NamespacedKey &key) const noexcept
    {
        const size_t h1 = std::hash<std::string_view>{}(key.getNamespace());
        const size_t h2 = std::hash<std::string_view>{}(key.getKey());
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

template <>
struct fmt::formatter<endstone::NamespacedKey> : formatter<string_view> {
    template <typename FormatContext>
    auto format(const endstone::NamespacedKey &val, FormatContext &ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}:{}", val.getNamespace(), val.getKey());
    }
};
