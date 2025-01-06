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

#ifdef __linux__

#include "endstone/core/hook.h"

#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <unistd.h>

#include <string>
#include <unordered_map>

#include <spdlog/spdlog.h>
#include <toml++/toml.h>

#include "endstone/core/os.h"

namespace {
void read_elf(const std::string &module_pathname, uint32_t section_type,
              const std::function<void(Elf *, GElf_Shdr &, GElf_Sym &)> &sym_handler)
{
    if (elf_version(EV_CURRENT) == EV_NONE) {
        throw std::runtime_error("ELF library initialization failed");
    }

    int fd = open(module_pathname.c_str(), O_RDONLY);
    if (fd < 0) {
        throw std::runtime_error("Failed to open file: " + module_pathname);
    }

    Elf *elf = elf_begin(fd, ELF_C_READ, nullptr);
    if (!elf) {
        throw std::runtime_error("elf_begin() failed.");
    }

    Elf_Scn *scn = nullptr;
    while ((scn = elf_nextscn(elf, scn)) != nullptr) {
        GElf_Shdr shdr;
        if (gelf_getshdr(scn, &shdr) != &shdr) {
            throw std::runtime_error("gelf_getshdr() failed.");
        }

        if (shdr.sh_type == section_type) {
            // Found the dynamic symbol table. Read it.
            Elf_Data *data = elf_getdata(scn, nullptr);
            const auto symbol_count = shdr.sh_size / shdr.sh_entsize;

            for (auto i = 0; i < symbol_count; ++i) {
                GElf_Sym sym;
                if (gelf_getsym(data, i, &sym) != &sym) {
                    throw std::runtime_error("gelf_getsym() failed.");
                }

                sym_handler(elf, shdr, sym);
            }

            break;  // No need to check further sections
        }
    }

    elf_end(elf);
    close(fd);
}
}  // namespace

namespace endstone::core::hook {

const std::unordered_map<std::string, void *> &get_detours()
{
    static std::unordered_map<std::string, void *> detours;
    if (!detours.empty()) {
        return detours;
    }

    auto *module_base = os::get_module_base();
    auto module_pathname = os::get_module_pathname();
    const auto &targets = get_targets();

    read_elf(module_pathname, SHT_DYNSYM, [&](auto *elf, auto &shdr, auto &sym) {
        if (sym.st_shndx == SHN_UNDEF || GELF_ST_TYPE(sym.st_info) != STT_FUNC ||
            GELF_ST_BIND(sym.st_info) != STB_GLOBAL) {
            return;
        }

        char *name = elf_strptr(elf, shdr.sh_link, sym.st_name);
        if (name == nullptr) {
            return;
        }

        auto offset = sym.st_value;
        spdlog::debug("D: {} -> 0x{:x}", name, offset);
        auto *detour = static_cast<char *>(module_base) + offset;
        detours.emplace(name, detour);
    });
    return detours;
}

const std::unordered_map<std::string, void *> &get_targets()
{
    static std::unordered_map<std::string, void *> targets;
    if (!targets.empty()) {
        return targets;
    }

    auto *executable_base = os::get_executable_base();
    const auto module_pathname = os::get_module_pathname();
    auto symbol_path = std::filesystem::path{module_pathname}.parent_path() / "symbols.toml";
    auto tbl = toml::parse_file(symbol_path.string());
    tbl["linux"].as_table()->for_each([executable_base](const toml::key &key, auto &&val) {
        if constexpr (toml::is_integer<decltype(val)>) {
            auto offset = val.get();
            spdlog::debug("T: {} -> 0x{:x}", key.data(), offset);
            auto *target = static_cast<char *>(executable_base) + offset;
            targets.emplace(key.data(), target);
        }
    });

    return targets;
}
}  // namespace endstone::core::hook

#endif
