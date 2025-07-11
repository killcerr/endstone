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

#include <memory>

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "endstone/namespaced_key.h"
#include "endstone/registry.h"

namespace py = pybind11;

namespace endstone::python {
template <typename T>
void registry(py::module &m, const std::string &name)
{
    py::class_<Registry<T>>(m, name.c_str())
        .def("get", py::overload_cast<NamespacedKey>(&Registry<T>::get), py::arg("key"),
             py::return_value_policy::reference)
        .def(
            "get_or_throw",
            [](const Registry<T> &self, NamespacedKey key) -> const T & {
                if (auto *p = self.get(key)) {
                    return *p;
                }
                throw py::key_error("No registry entry found for key: " + key.toString());
            },
            py::arg("key"), py::return_value_policy::reference)
        .def(
            "__getitem__",
            [](const Registry<T> &self, NamespacedKey key) -> const T & {
                if (auto *p = self.get(key)) {
                    return *p;
                }
                throw py::key_error("No registry entry found for key: " + key.toString());
            },
            py::arg("key"), py::return_value_policy::reference)
        .def(
            "__getitem__",
            [](const Registry<T> &self, const std::string &keyStr) -> const T & {
                auto key = NamespacedKey::fromString(keyStr);
                if (!key) {
                    throw std::runtime_error(key.error());
                }
                if (auto *p = self.get(key.value())) {
                    return *p;
                }
                throw py::key_error("No registry entry found for key: " + keyStr);
            },
            py::arg("key"), py::return_value_policy::reference)
        .def(
            "__iter__",
            [](const Registry<T> &self) {
                py::list items;
                self.forEach([&items](const T &elem) {
                    items.append(elem);
                    return true;  // continue iteration
                });
                return items;
            },
            py::return_value_policy::reference_internal)
        .def(
            "__contains__", [](const Registry<T> &self, const NamespacedKey key) { return self.get(key) != nullptr; },
            py::arg("key"))
        .def(
            "__contains__",
            [](const Registry<T> &self, const std::string &key_str) {
                auto key = NamespacedKey::fromString(key_str);
                return key.has_value() && self.get(key.value()) != nullptr;
            },
            py::arg("key"));
}
};  // namespace endstone::python
