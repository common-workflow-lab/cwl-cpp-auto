// SPDX-FileCopyrightText: Copyright 2016-2024 CWL Project Contributors
// SPDX-License-Identifier: Apache-2.0
#pragma once

/* This file was generated using schema-salad code generator.
 *
 * The embedded document is subject to the license of the original schema.
 */

#include <any>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace w3id_org::cwl {

struct store_config {
    bool simplifyTypes = true;
    bool transformListsToMaps = true;
    bool generateTags = false;
};

inline auto simplifyType(YAML::Node type, store_config const& config) -> YAML::Node {
    if (!config.simplifyTypes) return type;
    auto is_optional = [](YAML::Node const & node) {
        return node.IsSequence() && node.size() == 2u && node[0].Scalar() == "null";
    };

    auto is_array = [](YAML::Node const & node) {
        return node.IsMap() && node["type"].Scalar() == "array" && node["items"].IsScalar();
    };

    // 1. Collapsing optional scalar types into one option
    if (is_optional(type) && type[1].IsScalar()) {
        type = type[1].as<std::string>() + "?";
    }

    // 2. Collapsing array types into one option
    if (is_array(type)) {
        type = type["items"].as<std::string>() + "[]";
    }

    // 3. Collapsing optional array types into one option
    if (is_optional(type) && is_array(type[1])) {
        type = type[1]["items"].as<std::string>() + "[]?";
    }

    return type;
}

inline auto expandType(YAML::Node type) -> YAML::Node {
    auto ends_with = [](std::string str, std::string suffix) {
        if (str.size() < suffix.size()) return false;
        auto str_suffix = str.substr(str.size()-suffix.size(), suffix.size());
        return str_suffix == suffix;
    };

    // 0. If not a scalar type, nothing to do
    if (!type.IsDefined() || !type.IsScalar()) {
        return type;
    }

    auto str = type.as<std::string>();
    // 1. Check if optional array type and expand
    if (ends_with(str, "[]?")) {
        auto result = YAML::Node{};
        result.push_back(YAML::Node{"null"});
        auto array = YAML::Node{};
        array["type"] = "array";
        array["items"] = expandType(YAML::Node(str.substr(0, str.size()-3)));
        result.push_back(array);
        return result;
    }

    // 2. Expand array
    if (ends_with(str, "[]")) {
        auto array = YAML::Node{};
        array["type"] = "array";
        array["items"] = expandType(YAML::Node(str.substr(0, str.size()-2)));
        return array;
    }

    // 3. Expand optional scalar type
    if (ends_with(str, "?")) {
        auto result = YAML::Node{};
        result.push_back(YAML::Node{"null"});
        result.push_back(expandType(YAML::Node(str.substr(0, str.size()-1))));
        return result;
    }
    return type;
}

inline auto mergeYaml(YAML::Node n1, YAML::Node n2) {
    for (auto const& e : n2) {
        n1[e.first.as<std::string>()] = e.second;
    }
    return n1;
}

// declaring toYaml
inline auto toYaml(bool v, [[maybe_unused]] store_config const&) { return YAML::Node{v}; }
inline auto toYaml(float v, [[maybe_unused]] store_config const&) { return YAML::Node{v}; }
inline auto toYaml(double v, [[maybe_unused]] store_config const&) { return YAML::Node{v}; }
inline auto toYaml(char v, [[maybe_unused]] store_config const&) { return YAML::Node{v}; }
inline auto toYaml(int8_t v, [[maybe_unused]] store_config const&) { return YAML::Node{v}; }
inline auto toYaml(uint8_t v, [[maybe_unused]] store_config const&) { return YAML::Node{v}; }
inline auto toYaml(int16_t v, [[maybe_unused]] store_config const&) { return YAML::Node{v}; }
inline auto toYaml(uint16_t v, [[maybe_unused]] store_config const&) { return YAML::Node{v}; }
inline auto toYaml(int32_t v, [[maybe_unused]] store_config const&) { return YAML::Node{v}; }
inline auto toYaml(uint32_t v, [[maybe_unused]] store_config const&) { return YAML::Node{v}; }
inline auto toYaml(int64_t v, [[maybe_unused]] store_config const&) { return YAML::Node{v}; }
inline auto toYaml(uint64_t v, [[maybe_unused]] store_config const&) { return YAML::Node{v}; }
inline auto toYaml(std::monostate const&, [[maybe_unused]] store_config const&) {
    return YAML::Node(YAML::NodeType::Undefined);
}
inline auto toYaml(std::string const& v, [[maybe_unused]] store_config const&) {
    return YAML::Node{v};
}

template <typename T, typename ...Args>
auto anyToYaml_impl(std::any const& a, [[maybe_unused]] store_config const& config) {
    if (auto v = std::any_cast<T const>(&a)) {
        return toYaml(*v, config);
    }
    if constexpr (sizeof...(Args) > 0) {
        return anyToYaml_impl<Args...>(a, config);
    }
    return toYaml(std::monostate{}, config);
}

inline auto toYaml(std::any const& a, [[maybe_unused]] store_config const& config) {
    return anyToYaml_impl<bool,
                          float,
                          double,
                          char,
                          int8_t,
                          uint8_t,
                          int16_t,
                          uint16_t,
                          int32_t,
                          uint32_t,
                          int64_t,
                          uint64_t,
                          std::string>(a, config);
}

// declaring fromYaml
inline void fromYaml(YAML::Node const& n, bool& v) {
    v = n.as<bool>();
}
inline void fromYaml(YAML::Node const& n, float& v) {
    v = n.as<float>();
}
inline void fromYaml(YAML::Node const& n, double& v) {
    v = n.as<double>();
}
inline void fromYaml(YAML::Node const& n, int32_t& v) {
    v = n.as<int32_t>();
}
inline void fromYaml(YAML::Node const& n, int64_t& v) {
    v = n.as<int64_t>();
}
inline void fromYaml(YAML::Node const& n, std::string& v) {
    v = n.as<std::string>();
}
inline void fromYaml(YAML::Node const&, std::any&) {
}
inline void fromYaml(YAML::Node const&, std::monostate&) {
}

inline void addYamlField(YAML::Node& node, std::string const& key, YAML::Node value) {
    if (value.IsDefined()) {
        node[key] = value;
    }
}

inline auto convertListToMap(YAML::Node list, std::string const& mapSubject,
                             std::string const& mapPredicate, store_config const& config) {
    if (!config.transformListsToMaps) return list;
    if (mapSubject.empty()) return list;
    if (list.size() == 0) return list;
    auto map = YAML::Node{};
    for (YAML::Node n : list) {
        auto key = n[mapSubject].as<std::string>();
        if (mapPredicate.empty() || n[mapPredicate].IsMap() || n.size() > 2) {
            n.remove(mapSubject);
            map[key] = n;
        } else {
            map[key] = n[mapPredicate];
        }
    }
    return map;
}
inline auto convertMapToList(YAML::Node map, std::string const& mapSubject,
                             std::string const& mapPredicate) {
    if (mapSubject.empty()) return map;
    if (!map.IsDefined()) return map;
    if (!map.IsMap()) return map;
    auto list = YAML::Node{};
    for (auto n : map) {
        if (mapPredicate.empty() || n.second.IsMap()) {
            n.second[mapSubject] = n.first;
            list.push_back(n.second);
        } else {
            auto n2 = YAML::Node{};
            n2[mapSubject] = n.first;
            n2[mapPredicate] = n.second;
            list.push_back(n2);
        }
    }
    return list;
}

template <typename T> struct IsConstant : std::false_type {};

// fwd declaring toYaml
template <typename T>
auto toYaml(std::vector<T> const& v, [[maybe_unused]] store_config const& config) -> YAML::Node;
template <typename T>
auto toYaml(std::map<std::string, T> const& v, [[maybe_unused]] store_config const& config) -> YAML::Node;
template <typename T>
auto toYaml(T const& t, [[maybe_unused]] store_config const& config) -> YAML::Node;
template <typename ...Args>
auto toYaml(std::variant<Args...> const& t, [[maybe_unused]] store_config const& config) -> YAML::Node;

// fwd declaring fromYaml
template <typename T>
void fromYaml(YAML::Node const& n, std::vector<T>& v);
template <typename T>
void fromYaml(YAML::Node const& n, std::map<std::string, T>& v);
template <typename T>
void fromYaml(YAML::Node const& n, T& t);
template <typename ...Args>
void fromYaml(YAML::Node const& n, std::variant<Args...>& t);

template <typename T>
struct DetectAndExtractFromYaml {
    auto operator()(YAML::Node const&) const -> std::optional<T> {
        return std::nullopt;
    }
};

// special cwl expression string
struct cwl_expression_string {
    std::string s;

    auto toYaml([[maybe_unused]] store_config const& config) const {
        auto n = YAML::Node{s};
        if (config.generateTags) {
            n.SetTag("Expression");
        }
        return n;
    }
    void fromYaml(YAML::Node const& n) {
        s = n.as<std::string>();
    }
};


template <>
struct DetectAndExtractFromYaml<std::monostate> {
    auto operator()(YAML::Node const& n) const -> std::optional<std::monostate> {
        if (!n.IsDefined()) return std::monostate{};
        return std::nullopt;
    }
};

template <typename S>
struct DetectAndExtractFromYaml_implScalar {
    auto operator()(YAML::Node const& n) const -> std::optional<S> {
        try {
            if (n.IsScalar()) return n.as<S>();
        } catch(...) {}
        return std::nullopt;
    }
};

template <> struct DetectAndExtractFromYaml<bool>        : DetectAndExtractFromYaml_implScalar<bool>{};
template <> struct DetectAndExtractFromYaml<float>       : DetectAndExtractFromYaml_implScalar<float>{};
template <> struct DetectAndExtractFromYaml<double>      : DetectAndExtractFromYaml_implScalar<double>{};
template <> struct DetectAndExtractFromYaml<int32_t>     : DetectAndExtractFromYaml_implScalar<int32_t>{};
template <> struct DetectAndExtractFromYaml<int64_t>     : DetectAndExtractFromYaml_implScalar<int64_t>{};
template <> struct DetectAndExtractFromYaml<std::string> : DetectAndExtractFromYaml_implScalar<std::string>{};

template <typename T>
struct DetectAndExtractFromYaml<std::vector<T>> {
    auto operator()(YAML::Node const& n) const -> std::optional<std::vector<T>> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsSequence()) return std::nullopt;
        auto res = std::vector<T>{};
        fromYaml(n, res);
        return res;
    }
};

template <typename T>
struct DetectAndExtractFromYaml<std::map<std::string, T>> {
    auto operator()(YAML::Node const& n) const -> std::optional<std::map<std::string, T>> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = std::map<std::string, T>{};
        fromYaml(n, res);
        return res;
    }
};

template <typename T>
class heap_object {
    std::unique_ptr<T> data = std::make_unique<T>();

public:
    using value_t = T;
    heap_object() noexcept(false) = default;
    heap_object(heap_object const& oth) {
        *data = *oth;
    }
    heap_object(heap_object&& oth) noexcept(noexcept(*data = std::move(*oth))) {
        *data = std::move(*oth);
    }

    template <typename T2>
    heap_object(T2 const& oth) {
        *data = oth;
    }
    template <typename T2>
    heap_object(T2&& oth) noexcept(noexcept(*data = std::forward<T2>(oth))) {
        *data = std::forward<T2>(oth);
    }

    ~heap_object();

    auto operator=(heap_object const& oth) -> heap_object& {
        *data = *oth;
        return *this;
    }
    auto operator=(heap_object&& oth) noexcept(noexcept(*data = std::move(*oth))) -> heap_object& {
        *data = std::move(*oth);
        return *this;
    }

    template <typename T2>
    auto operator=(T2 const& oth) -> heap_object& {
        *data = oth;
        return *this;
    }
    template <typename T2>
    auto operator=(T2&& oth) noexcept(noexcept(*data = std::forward<T2>(oth))) -> heap_object& {
        *data = std::forward<T2>(oth);
        return *this;
    }

    auto operator->() noexcept(true) -> T* {
        return data.get();
    }
    auto operator->() const noexcept(true) -> T const* {
        return data.get();
    }
    auto operator*() noexcept(true) -> T& {
        return *data;
    }
    auto operator*() const noexcept(true) -> T const& {
        return *data;
    }
};

}
namespace w3id_org::cwl::salad { struct Documented; }
namespace w3id_org::cwl::salad { struct RecordField; }
namespace w3id_org::cwl::salad { struct RecordSchema; }
namespace w3id_org::cwl::salad { struct EnumSchema; }
namespace w3id_org::cwl::salad { struct ArraySchema; }
namespace w3id_org::cwl::cwl { struct File; }
namespace w3id_org::cwl::cwl { struct Directory; }
namespace w3id_org::cwl::cwl { struct Labeled; }
namespace w3id_org::cwl::cwl { struct Identified; }
namespace w3id_org::cwl::cwl { struct LoadContents; }
namespace w3id_org::cwl::cwl { struct FieldBase; }
namespace w3id_org::cwl::cwl { struct InputFormat; }
namespace w3id_org::cwl::cwl { struct OutputFormat; }
namespace w3id_org::cwl::cwl { struct Parameter; }
namespace w3id_org::cwl::cwl { struct InputBinding; }
namespace w3id_org::cwl::cwl { struct IOSchema; }
namespace w3id_org::cwl::cwl { struct InputSchema; }
namespace w3id_org::cwl::cwl { struct OutputSchema; }
namespace w3id_org::cwl::cwl { struct InputRecordField; }
namespace w3id_org::cwl::cwl { struct InputRecordSchema; }
namespace w3id_org::cwl::cwl { struct InputEnumSchema; }
namespace w3id_org::cwl::cwl { struct InputArraySchema; }
namespace w3id_org::cwl::cwl { struct OutputRecordField; }
namespace w3id_org::cwl::cwl { struct OutputRecordSchema; }
namespace w3id_org::cwl::cwl { struct OutputEnumSchema; }
namespace w3id_org::cwl::cwl { struct OutputArraySchema; }
namespace w3id_org::cwl::cwl { struct InputParameter; }
namespace w3id_org::cwl::cwl { struct OutputParameter; }
namespace w3id_org::cwl::cwl { struct ProcessRequirement; }
namespace w3id_org::cwl::cwl { struct Process; }
namespace w3id_org::cwl::cwl { struct InlineJavascriptRequirement; }
namespace w3id_org::cwl::cwl { struct CommandInputSchema; }
namespace w3id_org::cwl::cwl { struct SchemaDefRequirement; }
namespace w3id_org::cwl::cwl { struct SecondaryFileSchema; }
namespace w3id_org::cwl::cwl { struct LoadListingRequirement; }
namespace w3id_org::cwl::cwl { struct EnvironmentDef; }
namespace w3id_org::cwl::cwl { struct CommandLineBinding; }
namespace w3id_org::cwl::cwl { struct CommandOutputBinding; }
namespace w3id_org::cwl::cwl { struct CommandLineBindable; }
namespace w3id_org::cwl::cwl { struct CommandInputRecordField; }
namespace w3id_org::cwl::cwl { struct CommandInputRecordSchema; }
namespace w3id_org::cwl::cwl { struct CommandInputEnumSchema; }
namespace w3id_org::cwl::cwl { struct CommandInputArraySchema; }
namespace w3id_org::cwl::cwl { struct CommandOutputRecordField; }
namespace w3id_org::cwl::cwl { struct CommandOutputRecordSchema; }
namespace w3id_org::cwl::cwl { struct CommandOutputEnumSchema; }
namespace w3id_org::cwl::cwl { struct CommandOutputArraySchema; }
namespace w3id_org::cwl::cwl { struct CommandInputParameter; }
namespace w3id_org::cwl::cwl { struct CommandOutputParameter; }
namespace w3id_org::cwl::cwl { struct CommandLineTool; }
namespace w3id_org::cwl::cwl { struct DockerRequirement; }
namespace w3id_org::cwl::cwl { struct SoftwareRequirement; }
namespace w3id_org::cwl::cwl { struct SoftwarePackage; }
namespace w3id_org::cwl::cwl { struct Dirent; }
namespace w3id_org::cwl::cwl { struct InitialWorkDirRequirement; }
namespace w3id_org::cwl::cwl { struct EnvVarRequirement; }
namespace w3id_org::cwl::cwl { struct ShellCommandRequirement; }
namespace w3id_org::cwl::cwl { struct ResourceRequirement; }
namespace w3id_org::cwl::cwl { struct WorkReuse; }
namespace w3id_org::cwl::cwl { struct NetworkAccess; }
namespace w3id_org::cwl::cwl { struct InplaceUpdateRequirement; }
namespace w3id_org::cwl::cwl { struct ToolTimeLimit; }
namespace w3id_org::cwl::cwl { struct ExpressionToolOutputParameter; }
namespace w3id_org::cwl::cwl { struct WorkflowInputParameter; }
namespace w3id_org::cwl::cwl { struct ExpressionTool; }
namespace w3id_org::cwl::cwl { struct WorkflowOutputParameter; }
namespace w3id_org::cwl::cwl { struct Sink; }
namespace w3id_org::cwl::cwl { struct WorkflowStepInput; }
namespace w3id_org::cwl::cwl { struct WorkflowStepOutput; }
namespace w3id_org::cwl::cwl { struct WorkflowStep; }
namespace w3id_org::cwl::cwl { struct Workflow; }
namespace w3id_org::cwl::cwl { struct SubworkflowFeatureRequirement; }
namespace w3id_org::cwl::cwl { struct ScatterFeatureRequirement; }
namespace w3id_org::cwl::cwl { struct MultipleInputFeatureRequirement; }
namespace w3id_org::cwl::cwl { struct StepInputExpressionRequirement; }
namespace w3id_org::cwl::cwl { struct OperationInputParameter; }
namespace w3id_org::cwl::cwl { struct OperationOutputParameter; }
namespace w3id_org::cwl::cwl { struct Operation; }
namespace w3id_org::cwl::salad {
enum class PrimitiveType : unsigned int {
    null,
    boolean,
    int_,
    long_,
    float_,
    double_,
    string
};
inline auto to_string(PrimitiveType v) {
    static auto m = std::vector<std::string_view> {
        "null",
        "boolean",
        "int",
        "long",
        "float",
        "double",
        "string"
    };
    using U = std::underlying_type_t<w3id_org::cwl::salad::PrimitiveType>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::salad::PrimitiveType& out) {
    static auto m = std::map<std::string, w3id_org::cwl::salad::PrimitiveType, std::less<>> {
        {"null", w3id_org::cwl::salad::PrimitiveType::null},
        {"boolean", w3id_org::cwl::salad::PrimitiveType::boolean},
        {"int", w3id_org::cwl::salad::PrimitiveType::int_},
        {"long", w3id_org::cwl::salad::PrimitiveType::long_},
        {"float", w3id_org::cwl::salad::PrimitiveType::float_},
        {"double", w3id_org::cwl::salad::PrimitiveType::double_},
        {"string", w3id_org::cwl::salad::PrimitiveType::string},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::salad::PrimitiveType v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::salad::PrimitiveType");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::salad::PrimitiveType& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::salad::PrimitiveType> : std::true_type {};
}

namespace w3id_org::cwl::salad {
enum class Any : unsigned int {
    Any
};
inline auto to_string(Any v) {
    static auto m = std::vector<std::string_view> {
        "Any"
    };
    using U = std::underlying_type_t<w3id_org::cwl::salad::Any>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::salad::Any& out) {
    static auto m = std::map<std::string, w3id_org::cwl::salad::Any, std::less<>> {
        {"Any", w3id_org::cwl::salad::Any::Any},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::salad::Any v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::salad::Any");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::salad::Any& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::salad::Any> : std::true_type {};
}

namespace w3id_org::cwl::salad {
enum class RecordSchema_type_Record_name : unsigned int {
    record
};
inline auto to_string(RecordSchema_type_Record_name v) {
    static auto m = std::vector<std::string_view> {
        "record"
    };
    using U = std::underlying_type_t<w3id_org::cwl::salad::RecordSchema_type_Record_name>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::salad::RecordSchema_type_Record_name& out) {
    static auto m = std::map<std::string, w3id_org::cwl::salad::RecordSchema_type_Record_name, std::less<>> {
        {"record", w3id_org::cwl::salad::RecordSchema_type_Record_name::record},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::salad::RecordSchema_type_Record_name v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::salad::RecordSchema_type_Record_name");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::salad::RecordSchema_type_Record_name& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::salad::RecordSchema_type_Record_name> : std::true_type {};
}

namespace w3id_org::cwl::salad {
enum class EnumSchema_type_Enum_name : unsigned int {
    enum_
};
inline auto to_string(EnumSchema_type_Enum_name v) {
    static auto m = std::vector<std::string_view> {
        "enum"
    };
    using U = std::underlying_type_t<w3id_org::cwl::salad::EnumSchema_type_Enum_name>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::salad::EnumSchema_type_Enum_name& out) {
    static auto m = std::map<std::string, w3id_org::cwl::salad::EnumSchema_type_Enum_name, std::less<>> {
        {"enum", w3id_org::cwl::salad::EnumSchema_type_Enum_name::enum_},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::salad::EnumSchema_type_Enum_name v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::salad::EnumSchema_type_Enum_name");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::salad::EnumSchema_type_Enum_name& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::salad::EnumSchema_type_Enum_name> : std::true_type {};
}

namespace w3id_org::cwl::salad {
enum class ArraySchema_type_Array_name : unsigned int {
    array
};
inline auto to_string(ArraySchema_type_Array_name v) {
    static auto m = std::vector<std::string_view> {
        "array"
    };
    using U = std::underlying_type_t<w3id_org::cwl::salad::ArraySchema_type_Array_name>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::salad::ArraySchema_type_Array_name& out) {
    static auto m = std::map<std::string, w3id_org::cwl::salad::ArraySchema_type_Array_name, std::less<>> {
        {"array", w3id_org::cwl::salad::ArraySchema_type_Array_name::array},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::salad::ArraySchema_type_Array_name v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::salad::ArraySchema_type_Array_name");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::salad::ArraySchema_type_Array_name& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::salad::ArraySchema_type_Array_name> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class CWLVersion : unsigned int {
    draft_2,
    draft_3_dev1,
    draft_3_dev2,
    draft_3_dev3,
    draft_3_dev4,
    draft_3_dev5,
    draft_3,
    draft_4_dev1,
    draft_4_dev2,
    draft_4_dev3,
    v1_0_dev4,
    v1_0,
    v1_1_0_dev1,
    v1_1,
    v1_2_0_dev1,
    v1_2_0_dev2,
    v1_2_0_dev3,
    v1_2_0_dev4,
    v1_2_0_dev5,
    v1_2
};
inline auto to_string(CWLVersion v) {
    static auto m = std::vector<std::string_view> {
        "draft-2",
        "draft-3.dev1",
        "draft-3.dev2",
        "draft-3.dev3",
        "draft-3.dev4",
        "draft-3.dev5",
        "draft-3",
        "draft-4.dev1",
        "draft-4.dev2",
        "draft-4.dev3",
        "v1.0.dev4",
        "v1.0",
        "v1.1.0-dev1",
        "v1.1",
        "v1.2.0-dev1",
        "v1.2.0-dev2",
        "v1.2.0-dev3",
        "v1.2.0-dev4",
        "v1.2.0-dev5",
        "v1.2"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::CWLVersion>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::CWLVersion& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::CWLVersion, std::less<>> {
        {"draft-2", w3id_org::cwl::cwl::CWLVersion::draft_2},
        {"draft-3.dev1", w3id_org::cwl::cwl::CWLVersion::draft_3_dev1},
        {"draft-3.dev2", w3id_org::cwl::cwl::CWLVersion::draft_3_dev2},
        {"draft-3.dev3", w3id_org::cwl::cwl::CWLVersion::draft_3_dev3},
        {"draft-3.dev4", w3id_org::cwl::cwl::CWLVersion::draft_3_dev4},
        {"draft-3.dev5", w3id_org::cwl::cwl::CWLVersion::draft_3_dev5},
        {"draft-3", w3id_org::cwl::cwl::CWLVersion::draft_3},
        {"draft-4.dev1", w3id_org::cwl::cwl::CWLVersion::draft_4_dev1},
        {"draft-4.dev2", w3id_org::cwl::cwl::CWLVersion::draft_4_dev2},
        {"draft-4.dev3", w3id_org::cwl::cwl::CWLVersion::draft_4_dev3},
        {"v1.0.dev4", w3id_org::cwl::cwl::CWLVersion::v1_0_dev4},
        {"v1.0", w3id_org::cwl::cwl::CWLVersion::v1_0},
        {"v1.1.0-dev1", w3id_org::cwl::cwl::CWLVersion::v1_1_0_dev1},
        {"v1.1", w3id_org::cwl::cwl::CWLVersion::v1_1},
        {"v1.2.0-dev1", w3id_org::cwl::cwl::CWLVersion::v1_2_0_dev1},
        {"v1.2.0-dev2", w3id_org::cwl::cwl::CWLVersion::v1_2_0_dev2},
        {"v1.2.0-dev3", w3id_org::cwl::cwl::CWLVersion::v1_2_0_dev3},
        {"v1.2.0-dev4", w3id_org::cwl::cwl::CWLVersion::v1_2_0_dev4},
        {"v1.2.0-dev5", w3id_org::cwl::cwl::CWLVersion::v1_2_0_dev5},
        {"v1.2", w3id_org::cwl::cwl::CWLVersion::v1_2},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::CWLVersion v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::CWLVersion");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::CWLVersion& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::CWLVersion> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class CWLType : unsigned int {
    null,
    boolean,
    int_,
    long_,
    float_,
    double_,
    string,
    File,
    Directory
};
inline auto to_string(CWLType v) {
    static auto m = std::vector<std::string_view> {
        "null",
        "boolean",
        "int",
        "long",
        "float",
        "double",
        "string",
        "File",
        "Directory"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::CWLType>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::CWLType& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::CWLType, std::less<>> {
        {"null", w3id_org::cwl::cwl::CWLType::null},
        {"boolean", w3id_org::cwl::cwl::CWLType::boolean},
        {"int", w3id_org::cwl::cwl::CWLType::int_},
        {"long", w3id_org::cwl::cwl::CWLType::long_},
        {"float", w3id_org::cwl::cwl::CWLType::float_},
        {"double", w3id_org::cwl::cwl::CWLType::double_},
        {"string", w3id_org::cwl::cwl::CWLType::string},
        {"File", w3id_org::cwl::cwl::CWLType::File},
        {"Directory", w3id_org::cwl::cwl::CWLType::Directory},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::CWLType v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::CWLType");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::CWLType& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::CWLType> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class File_class_File_class : unsigned int {
    File
};
inline auto to_string(File_class_File_class v) {
    static auto m = std::vector<std::string_view> {
        "File"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::File_class_File_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::File_class_File_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::File_class_File_class, std::less<>> {
        {"File", w3id_org::cwl::cwl::File_class_File_class::File},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::File_class_File_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::File_class_File_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::File_class_File_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::File_class_File_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class Directory_class_Directory_class : unsigned int {
    Directory
};
inline auto to_string(Directory_class_Directory_class v) {
    static auto m = std::vector<std::string_view> {
        "Directory"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::Directory_class_Directory_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::Directory_class_Directory_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::Directory_class_Directory_class, std::less<>> {
        {"Directory", w3id_org::cwl::cwl::Directory_class_Directory_class::Directory},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::Directory_class_Directory_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::Directory_class_Directory_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::Directory_class_Directory_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::Directory_class_Directory_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class LoadListingEnum : unsigned int {
    no_listing,
    shallow_listing,
    deep_listing
};
inline auto to_string(LoadListingEnum v) {
    static auto m = std::vector<std::string_view> {
        "no_listing",
        "shallow_listing",
        "deep_listing"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::LoadListingEnum>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::LoadListingEnum& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::LoadListingEnum, std::less<>> {
        {"no_listing", w3id_org::cwl::cwl::LoadListingEnum::no_listing},
        {"shallow_listing", w3id_org::cwl::cwl::LoadListingEnum::shallow_listing},
        {"deep_listing", w3id_org::cwl::cwl::LoadListingEnum::deep_listing},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::LoadListingEnum v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::LoadListingEnum");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::LoadListingEnum& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::LoadListingEnum> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class Expression : unsigned int {
    ExpressionPlaceholder
};
inline auto to_string(Expression v) {
    static auto m = std::vector<std::string_view> {
        "ExpressionPlaceholder"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::Expression>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::Expression& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::Expression, std::less<>> {
        {"ExpressionPlaceholder", w3id_org::cwl::cwl::Expression::ExpressionPlaceholder},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::Expression v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::Expression");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::Expression& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::Expression> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class InlineJavascriptRequirement_class_InlineJavascriptRequirement_class : unsigned int {
    InlineJavascriptRequirement
};
inline auto to_string(InlineJavascriptRequirement_class_InlineJavascriptRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "InlineJavascriptRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class, std::less<>> {
        {"InlineJavascriptRequirement", w3id_org::cwl::cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class::InlineJavascriptRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class SchemaDefRequirement_class_SchemaDefRequirement_class : unsigned int {
    SchemaDefRequirement
};
inline auto to_string(SchemaDefRequirement_class_SchemaDefRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "SchemaDefRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::SchemaDefRequirement_class_SchemaDefRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::SchemaDefRequirement_class_SchemaDefRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::SchemaDefRequirement_class_SchemaDefRequirement_class, std::less<>> {
        {"SchemaDefRequirement", w3id_org::cwl::cwl::SchemaDefRequirement_class_SchemaDefRequirement_class::SchemaDefRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::SchemaDefRequirement_class_SchemaDefRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::SchemaDefRequirement_class_SchemaDefRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::SchemaDefRequirement_class_SchemaDefRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::SchemaDefRequirement_class_SchemaDefRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class LoadListingRequirement_class_LoadListingRequirement_class : unsigned int {
    LoadListingRequirement
};
inline auto to_string(LoadListingRequirement_class_LoadListingRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "LoadListingRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::LoadListingRequirement_class_LoadListingRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::LoadListingRequirement_class_LoadListingRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::LoadListingRequirement_class_LoadListingRequirement_class, std::less<>> {
        {"LoadListingRequirement", w3id_org::cwl::cwl::LoadListingRequirement_class_LoadListingRequirement_class::LoadListingRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::LoadListingRequirement_class_LoadListingRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::LoadListingRequirement_class_LoadListingRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::LoadListingRequirement_class_LoadListingRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::LoadListingRequirement_class_LoadListingRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class stdin_ : unsigned int {
    stdin_
};
inline auto to_string(stdin_ v) {
    static auto m = std::vector<std::string_view> {
        "stdin"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::stdin_>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::stdin_& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::stdin_, std::less<>> {
        {"stdin", w3id_org::cwl::cwl::stdin_::stdin_},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::stdin_ v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::stdin_");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::stdin_& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::stdin_> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class stdout_ : unsigned int {
    stdout_
};
inline auto to_string(stdout_ v) {
    static auto m = std::vector<std::string_view> {
        "stdout"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::stdout_>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::stdout_& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::stdout_, std::less<>> {
        {"stdout", w3id_org::cwl::cwl::stdout_::stdout_},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::stdout_ v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::stdout_");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::stdout_& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::stdout_> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class stderr_ : unsigned int {
    stderr_
};
inline auto to_string(stderr_ v) {
    static auto m = std::vector<std::string_view> {
        "stderr"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::stderr_>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::stderr_& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::stderr_, std::less<>> {
        {"stderr", w3id_org::cwl::cwl::stderr_::stderr_},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::stderr_ v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::stderr_");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::stderr_& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::stderr_> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class CommandLineTool_class_CommandLineTool_class : unsigned int {
    CommandLineTool
};
inline auto to_string(CommandLineTool_class_CommandLineTool_class v) {
    static auto m = std::vector<std::string_view> {
        "CommandLineTool"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::CommandLineTool_class_CommandLineTool_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::CommandLineTool_class_CommandLineTool_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::CommandLineTool_class_CommandLineTool_class, std::less<>> {
        {"CommandLineTool", w3id_org::cwl::cwl::CommandLineTool_class_CommandLineTool_class::CommandLineTool},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::CommandLineTool_class_CommandLineTool_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::CommandLineTool_class_CommandLineTool_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::CommandLineTool_class_CommandLineTool_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::CommandLineTool_class_CommandLineTool_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class DockerRequirement_class_DockerRequirement_class : unsigned int {
    DockerRequirement
};
inline auto to_string(DockerRequirement_class_DockerRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "DockerRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::DockerRequirement_class_DockerRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::DockerRequirement_class_DockerRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::DockerRequirement_class_DockerRequirement_class, std::less<>> {
        {"DockerRequirement", w3id_org::cwl::cwl::DockerRequirement_class_DockerRequirement_class::DockerRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::DockerRequirement_class_DockerRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::DockerRequirement_class_DockerRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::DockerRequirement_class_DockerRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::DockerRequirement_class_DockerRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class SoftwareRequirement_class_SoftwareRequirement_class : unsigned int {
    SoftwareRequirement
};
inline auto to_string(SoftwareRequirement_class_SoftwareRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "SoftwareRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::SoftwareRequirement_class_SoftwareRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::SoftwareRequirement_class_SoftwareRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::SoftwareRequirement_class_SoftwareRequirement_class, std::less<>> {
        {"SoftwareRequirement", w3id_org::cwl::cwl::SoftwareRequirement_class_SoftwareRequirement_class::SoftwareRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::SoftwareRequirement_class_SoftwareRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::SoftwareRequirement_class_SoftwareRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::SoftwareRequirement_class_SoftwareRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::SoftwareRequirement_class_SoftwareRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class InitialWorkDirRequirement_class_InitialWorkDirRequirement_class : unsigned int {
    InitialWorkDirRequirement
};
inline auto to_string(InitialWorkDirRequirement_class_InitialWorkDirRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "InitialWorkDirRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class, std::less<>> {
        {"InitialWorkDirRequirement", w3id_org::cwl::cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class::InitialWorkDirRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class EnvVarRequirement_class_EnvVarRequirement_class : unsigned int {
    EnvVarRequirement
};
inline auto to_string(EnvVarRequirement_class_EnvVarRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "EnvVarRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::EnvVarRequirement_class_EnvVarRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::EnvVarRequirement_class_EnvVarRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::EnvVarRequirement_class_EnvVarRequirement_class, std::less<>> {
        {"EnvVarRequirement", w3id_org::cwl::cwl::EnvVarRequirement_class_EnvVarRequirement_class::EnvVarRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::EnvVarRequirement_class_EnvVarRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::EnvVarRequirement_class_EnvVarRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::EnvVarRequirement_class_EnvVarRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::EnvVarRequirement_class_EnvVarRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class ShellCommandRequirement_class_ShellCommandRequirement_class : unsigned int {
    ShellCommandRequirement
};
inline auto to_string(ShellCommandRequirement_class_ShellCommandRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "ShellCommandRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::ShellCommandRequirement_class_ShellCommandRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::ShellCommandRequirement_class_ShellCommandRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::ShellCommandRequirement_class_ShellCommandRequirement_class, std::less<>> {
        {"ShellCommandRequirement", w3id_org::cwl::cwl::ShellCommandRequirement_class_ShellCommandRequirement_class::ShellCommandRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::ShellCommandRequirement_class_ShellCommandRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::ShellCommandRequirement_class_ShellCommandRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::ShellCommandRequirement_class_ShellCommandRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::ShellCommandRequirement_class_ShellCommandRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class ResourceRequirement_class_ResourceRequirement_class : unsigned int {
    ResourceRequirement
};
inline auto to_string(ResourceRequirement_class_ResourceRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "ResourceRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::ResourceRequirement_class_ResourceRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::ResourceRequirement_class_ResourceRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::ResourceRequirement_class_ResourceRequirement_class, std::less<>> {
        {"ResourceRequirement", w3id_org::cwl::cwl::ResourceRequirement_class_ResourceRequirement_class::ResourceRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::ResourceRequirement_class_ResourceRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::ResourceRequirement_class_ResourceRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::ResourceRequirement_class_ResourceRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::ResourceRequirement_class_ResourceRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class WorkReuse_class_WorkReuse_class : unsigned int {
    WorkReuse
};
inline auto to_string(WorkReuse_class_WorkReuse_class v) {
    static auto m = std::vector<std::string_view> {
        "WorkReuse"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::WorkReuse_class_WorkReuse_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::WorkReuse_class_WorkReuse_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::WorkReuse_class_WorkReuse_class, std::less<>> {
        {"WorkReuse", w3id_org::cwl::cwl::WorkReuse_class_WorkReuse_class::WorkReuse},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::WorkReuse_class_WorkReuse_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::WorkReuse_class_WorkReuse_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::WorkReuse_class_WorkReuse_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::WorkReuse_class_WorkReuse_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class NetworkAccess_class_NetworkAccess_class : unsigned int {
    NetworkAccess
};
inline auto to_string(NetworkAccess_class_NetworkAccess_class v) {
    static auto m = std::vector<std::string_view> {
        "NetworkAccess"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::NetworkAccess_class_NetworkAccess_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::NetworkAccess_class_NetworkAccess_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::NetworkAccess_class_NetworkAccess_class, std::less<>> {
        {"NetworkAccess", w3id_org::cwl::cwl::NetworkAccess_class_NetworkAccess_class::NetworkAccess},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::NetworkAccess_class_NetworkAccess_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::NetworkAccess_class_NetworkAccess_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::NetworkAccess_class_NetworkAccess_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::NetworkAccess_class_NetworkAccess_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class InplaceUpdateRequirement_class_InplaceUpdateRequirement_class : unsigned int {
    InplaceUpdateRequirement
};
inline auto to_string(InplaceUpdateRequirement_class_InplaceUpdateRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "InplaceUpdateRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class, std::less<>> {
        {"InplaceUpdateRequirement", w3id_org::cwl::cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class::InplaceUpdateRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class ToolTimeLimit_class_ToolTimeLimit_class : unsigned int {
    ToolTimeLimit
};
inline auto to_string(ToolTimeLimit_class_ToolTimeLimit_class v) {
    static auto m = std::vector<std::string_view> {
        "ToolTimeLimit"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::ToolTimeLimit_class_ToolTimeLimit_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::ToolTimeLimit_class_ToolTimeLimit_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::ToolTimeLimit_class_ToolTimeLimit_class, std::less<>> {
        {"ToolTimeLimit", w3id_org::cwl::cwl::ToolTimeLimit_class_ToolTimeLimit_class::ToolTimeLimit},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::ToolTimeLimit_class_ToolTimeLimit_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::ToolTimeLimit_class_ToolTimeLimit_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::ToolTimeLimit_class_ToolTimeLimit_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::ToolTimeLimit_class_ToolTimeLimit_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class ExpressionTool_class_ExpressionTool_class : unsigned int {
    ExpressionTool
};
inline auto to_string(ExpressionTool_class_ExpressionTool_class v) {
    static auto m = std::vector<std::string_view> {
        "ExpressionTool"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::ExpressionTool_class_ExpressionTool_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::ExpressionTool_class_ExpressionTool_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::ExpressionTool_class_ExpressionTool_class, std::less<>> {
        {"ExpressionTool", w3id_org::cwl::cwl::ExpressionTool_class_ExpressionTool_class::ExpressionTool},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::ExpressionTool_class_ExpressionTool_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::ExpressionTool_class_ExpressionTool_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::ExpressionTool_class_ExpressionTool_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::ExpressionTool_class_ExpressionTool_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class LinkMergeMethod : unsigned int {
    merge_nested,
    merge_flattened
};
inline auto to_string(LinkMergeMethod v) {
    static auto m = std::vector<std::string_view> {
        "merge_nested",
        "merge_flattened"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::LinkMergeMethod>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::LinkMergeMethod& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::LinkMergeMethod, std::less<>> {
        {"merge_nested", w3id_org::cwl::cwl::LinkMergeMethod::merge_nested},
        {"merge_flattened", w3id_org::cwl::cwl::LinkMergeMethod::merge_flattened},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::LinkMergeMethod v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::LinkMergeMethod");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::LinkMergeMethod& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::LinkMergeMethod> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class PickValueMethod : unsigned int {
    first_non_null,
    the_only_non_null,
    all_non_null
};
inline auto to_string(PickValueMethod v) {
    static auto m = std::vector<std::string_view> {
        "first_non_null",
        "the_only_non_null",
        "all_non_null"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::PickValueMethod>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::PickValueMethod& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::PickValueMethod, std::less<>> {
        {"first_non_null", w3id_org::cwl::cwl::PickValueMethod::first_non_null},
        {"the_only_non_null", w3id_org::cwl::cwl::PickValueMethod::the_only_non_null},
        {"all_non_null", w3id_org::cwl::cwl::PickValueMethod::all_non_null},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::PickValueMethod v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::PickValueMethod");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::PickValueMethod& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::PickValueMethod> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class ScatterMethod : unsigned int {
    dotproduct,
    nested_crossproduct,
    flat_crossproduct
};
inline auto to_string(ScatterMethod v) {
    static auto m = std::vector<std::string_view> {
        "dotproduct",
        "nested_crossproduct",
        "flat_crossproduct"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::ScatterMethod>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::ScatterMethod& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::ScatterMethod, std::less<>> {
        {"dotproduct", w3id_org::cwl::cwl::ScatterMethod::dotproduct},
        {"nested_crossproduct", w3id_org::cwl::cwl::ScatterMethod::nested_crossproduct},
        {"flat_crossproduct", w3id_org::cwl::cwl::ScatterMethod::flat_crossproduct},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::ScatterMethod v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::ScatterMethod");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::ScatterMethod& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::ScatterMethod> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class Workflow_class_Workflow_class : unsigned int {
    Workflow
};
inline auto to_string(Workflow_class_Workflow_class v) {
    static auto m = std::vector<std::string_view> {
        "Workflow"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::Workflow_class_Workflow_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::Workflow_class_Workflow_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::Workflow_class_Workflow_class, std::less<>> {
        {"Workflow", w3id_org::cwl::cwl::Workflow_class_Workflow_class::Workflow},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::Workflow_class_Workflow_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::Workflow_class_Workflow_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::Workflow_class_Workflow_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::Workflow_class_Workflow_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class : unsigned int {
    SubworkflowFeatureRequirement
};
inline auto to_string(SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "SubworkflowFeatureRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class, std::less<>> {
        {"SubworkflowFeatureRequirement", w3id_org::cwl::cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class::SubworkflowFeatureRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class ScatterFeatureRequirement_class_ScatterFeatureRequirement_class : unsigned int {
    ScatterFeatureRequirement
};
inline auto to_string(ScatterFeatureRequirement_class_ScatterFeatureRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "ScatterFeatureRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class, std::less<>> {
        {"ScatterFeatureRequirement", w3id_org::cwl::cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class::ScatterFeatureRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class : unsigned int {
    MultipleInputFeatureRequirement
};
inline auto to_string(MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "MultipleInputFeatureRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class, std::less<>> {
        {"MultipleInputFeatureRequirement", w3id_org::cwl::cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class::MultipleInputFeatureRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class StepInputExpressionRequirement_class_StepInputExpressionRequirement_class : unsigned int {
    StepInputExpressionRequirement
};
inline auto to_string(StepInputExpressionRequirement_class_StepInputExpressionRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "StepInputExpressionRequirement"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class, std::less<>> {
        {"StepInputExpressionRequirement", w3id_org::cwl::cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class::StepInputExpressionRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class> : std::true_type {};
}

namespace w3id_org::cwl::cwl {
enum class Operation_class_Operation_class : unsigned int {
    Operation
};
inline auto to_string(Operation_class_Operation_class v) {
    static auto m = std::vector<std::string_view> {
        "Operation"
    };
    using U = std::underlying_type_t<w3id_org::cwl::cwl::Operation_class_Operation_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, w3id_org::cwl::cwl::Operation_class_Operation_class& out) {
    static auto m = std::map<std::string, w3id_org::cwl::cwl::Operation_class_Operation_class, std::less<>> {
        {"Operation", w3id_org::cwl::cwl::Operation_class_Operation_class::Operation},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
namespace w3id_org::cwl {
inline auto toYaml(w3id_org::cwl::cwl::Operation_class_Operation_class v, [[maybe_unused]] ::w3id_org::cwl::store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("w3id_org::cwl::cwl::Operation_class_Operation_class");
    return n;
}
inline void fromYaml(YAML::Node n, w3id_org::cwl::cwl::Operation_class_Operation_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<w3id_org::cwl::cwl::Operation_class_Operation_class> : std::true_type {};
}

namespace w3id_org::cwl::salad {
struct Documented {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    virtual ~Documented() = 0;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::salad {
struct RecordField
    : w3id_org::cwl::salad::Documented {
    heap_object<std::string> name;
    heap_object<std::variant<std::variant<bool, int32_t, int64_t, float, double, std::string>, RecordSchema, EnumSchema, ArraySchema, std::string, std::vector<std::variant<std::variant<bool, int32_t, int64_t, float, double, std::string>, RecordSchema, EnumSchema, ArraySchema, std::string>>>> type;
    ~RecordField() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::salad {
struct RecordSchema {
    heap_object<std::variant<std::monostate, std::vector<RecordField>>> fields;
    heap_object<RecordSchema_type_Record_name> type;
    virtual ~RecordSchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::salad {
struct EnumSchema {
    heap_object<std::variant<std::monostate, std::string>> name;
    heap_object<std::vector<std::string>> symbols;
    heap_object<EnumSchema_type_Enum_name> type;
    virtual ~EnumSchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::salad {
struct ArraySchema {
    heap_object<std::variant<std::variant<bool, int32_t, int64_t, float, double, std::string>, RecordSchema, EnumSchema, ArraySchema, std::string, std::vector<std::variant<std::variant<bool, int32_t, int64_t, float, double, std::string>, RecordSchema, EnumSchema, ArraySchema, std::string>>>> items;
    heap_object<ArraySchema_type_Array_name> type;
    virtual ~ArraySchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct File {
    heap_object<File_class_File_class> class_;
    heap_object<std::variant<std::monostate, std::string>> location;
    heap_object<std::variant<std::monostate, std::string>> path;
    heap_object<std::variant<std::monostate, std::string>> basename;
    heap_object<std::variant<std::monostate, std::string>> dirname;
    heap_object<std::variant<std::monostate, std::string>> nameroot;
    heap_object<std::variant<std::monostate, std::string>> nameext;
    heap_object<std::variant<std::monostate, std::string>> checksum;
    heap_object<std::variant<std::monostate, int32_t, int64_t>> size;
    heap_object<std::variant<std::monostate, std::vector<std::variant<File, Directory>>>> secondaryFiles;
    heap_object<std::variant<std::monostate, std::string>> format;
    heap_object<std::variant<std::monostate, std::string>> contents;
    virtual ~File() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct Directory {
    heap_object<Directory_class_Directory_class> class_;
    heap_object<std::variant<std::monostate, std::string>> location;
    heap_object<std::variant<std::monostate, std::string>> path;
    heap_object<std::variant<std::monostate, std::string>> basename;
    heap_object<std::variant<std::monostate, std::vector<std::variant<File, Directory>>>> listing;
    virtual ~Directory() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct Labeled {
    heap_object<std::variant<std::monostate, std::string>> label;
    virtual ~Labeled() = 0;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct Identified {
    heap_object<std::variant<std::monostate, std::string>> id;
    virtual ~Identified() = 0;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct LoadContents {
    heap_object<std::variant<std::monostate, bool>> loadContents;
    heap_object<std::variant<std::monostate, LoadListingEnum>> loadListing;
    virtual ~LoadContents() = 0;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct FieldBase
    : w3id_org::cwl::cwl::Labeled {
    heap_object<std::variant<std::monostate, SecondaryFileSchema, std::vector<SecondaryFileSchema>>> secondaryFiles;
    heap_object<std::variant<std::monostate, bool>> streamable;
    virtual ~FieldBase() = 0;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct InputFormat {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>, cwl_expression_string>> format;
    virtual ~InputFormat() = 0;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct OutputFormat {
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> format;
    virtual ~OutputFormat() = 0;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct Parameter
    : w3id_org::cwl::cwl::FieldBase
    , w3id_org::cwl::salad::Documented
    , w3id_org::cwl::cwl::Identified {
    virtual ~Parameter() = 0;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct InputBinding {
    heap_object<std::variant<std::monostate, bool>> loadContents;
    virtual ~InputBinding() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct IOSchema
    : w3id_org::cwl::cwl::Labeled
    , w3id_org::cwl::salad::Documented {
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~IOSchema() = 0;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct InputSchema
    : w3id_org::cwl::cwl::IOSchema {
    virtual ~InputSchema() = 0;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct OutputSchema
    : w3id_org::cwl::cwl::IOSchema {
    virtual ~OutputSchema() = 0;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct InputRecordField {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::string> name;
    heap_object<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string, std::vector<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string>>>> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, SecondaryFileSchema, std::vector<SecondaryFileSchema>>> secondaryFiles;
    heap_object<std::variant<std::monostate, bool>> streamable;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>, cwl_expression_string>> format;
    heap_object<std::variant<std::monostate, bool>> loadContents;
    heap_object<std::variant<std::monostate, LoadListingEnum>> loadListing;
    virtual ~InputRecordField() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct InputRecordSchema {
    heap_object<std::variant<std::monostate, std::vector<InputRecordField>>> fields;
    heap_object<w3id_org::cwl::salad::RecordSchema_type_Record_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~InputRecordSchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct InputEnumSchema
    : w3id_org::cwl::salad::EnumSchema
    , w3id_org::cwl::cwl::InputSchema {
    ~InputEnumSchema() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct InputArraySchema {
    heap_object<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string, std::vector<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string>>>> items;
    heap_object<w3id_org::cwl::salad::ArraySchema_type_Array_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~InputArraySchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct OutputRecordField {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::string> name;
    heap_object<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string, std::vector<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string>>>> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, SecondaryFileSchema, std::vector<SecondaryFileSchema>>> secondaryFiles;
    heap_object<std::variant<std::monostate, bool>> streamable;
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> format;
    virtual ~OutputRecordField() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct OutputRecordSchema {
    heap_object<std::variant<std::monostate, std::vector<OutputRecordField>>> fields;
    heap_object<w3id_org::cwl::salad::RecordSchema_type_Record_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~OutputRecordSchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct OutputEnumSchema
    : w3id_org::cwl::salad::EnumSchema
    , w3id_org::cwl::cwl::OutputSchema {
    ~OutputEnumSchema() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct OutputArraySchema {
    heap_object<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string, std::vector<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string>>>> items;
    heap_object<w3id_org::cwl::salad::ArraySchema_type_Array_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~OutputArraySchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct InputParameter
    : w3id_org::cwl::cwl::Parameter
    , w3id_org::cwl::cwl::InputFormat
    , w3id_org::cwl::cwl::LoadContents {
    heap_object<std::variant<std::monostate, File, Directory, std::any>> default_;
    virtual ~InputParameter() = 0;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct OutputParameter
    : w3id_org::cwl::cwl::Parameter
    , w3id_org::cwl::cwl::OutputFormat {
    virtual ~OutputParameter() = 0;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct ProcessRequirement {
    virtual ~ProcessRequirement() = 0;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct Process
    : w3id_org::cwl::cwl::Identified
    , w3id_org::cwl::cwl::Labeled
    , w3id_org::cwl::salad::Documented {
    heap_object<std::vector<std::variant<CommandInputParameter, WorkflowInputParameter, OperationInputParameter>>> inputs;
    heap_object<std::vector<std::variant<CommandOutputParameter, ExpressionToolOutputParameter, WorkflowOutputParameter, OperationOutputParameter>>> outputs;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement>>>> requirements;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement, std::any>>>> hints;
    heap_object<std::variant<std::monostate, CWLVersion>> cwlVersion;
    heap_object<std::variant<std::monostate, std::vector<std::string>>> intent;
    virtual ~Process() = 0;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct InlineJavascriptRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<InlineJavascriptRequirement_class_InlineJavascriptRequirement_class> class_;
    heap_object<std::variant<std::monostate, std::vector<std::string>>> expressionLib;
    ~InlineJavascriptRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct CommandInputSchema {
    virtual ~CommandInputSchema() = 0;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct SchemaDefRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<SchemaDefRequirement_class_SchemaDefRequirement_class> class_;
    heap_object<std::vector<std::variant<CommandInputRecordSchema, CommandInputEnumSchema, CommandInputArraySchema>>> types;
    ~SchemaDefRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct SecondaryFileSchema {
    heap_object<std::variant<std::string, cwl_expression_string>> pattern;
    heap_object<std::variant<std::monostate, bool, cwl_expression_string>> required;
    virtual ~SecondaryFileSchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct LoadListingRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<LoadListingRequirement_class_LoadListingRequirement_class> class_;
    heap_object<std::variant<std::monostate, LoadListingEnum>> loadListing;
    ~LoadListingRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct EnvironmentDef {
    heap_object<std::string> envName;
    heap_object<std::variant<std::string, cwl_expression_string>> envValue;
    virtual ~EnvironmentDef() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct CommandLineBinding
    : w3id_org::cwl::cwl::InputBinding {
    heap_object<std::variant<std::monostate, int32_t, cwl_expression_string>> position;
    heap_object<std::variant<std::monostate, std::string>> prefix;
    heap_object<std::variant<std::monostate, bool>> separate;
    heap_object<std::variant<std::monostate, std::string>> itemSeparator;
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> valueFrom;
    heap_object<std::variant<std::monostate, bool>> shellQuote;
    ~CommandLineBinding() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct CommandOutputBinding
    : w3id_org::cwl::cwl::LoadContents {
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string, std::vector<std::string>>> glob;
    heap_object<std::variant<std::monostate, cwl_expression_string>> outputEval;
    ~CommandOutputBinding() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct CommandLineBindable {
    heap_object<std::variant<std::monostate, CommandLineBinding>> inputBinding;
    virtual ~CommandLineBindable() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct CommandInputRecordField {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::string> name;
    heap_object<std::variant<CWLType, CommandInputRecordSchema, CommandInputEnumSchema, CommandInputArraySchema, std::string, std::vector<std::variant<CWLType, CommandInputRecordSchema, CommandInputEnumSchema, CommandInputArraySchema, std::string>>>> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, SecondaryFileSchema, std::vector<SecondaryFileSchema>>> secondaryFiles;
    heap_object<std::variant<std::monostate, bool>> streamable;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>, cwl_expression_string>> format;
    heap_object<std::variant<std::monostate, bool>> loadContents;
    heap_object<std::variant<std::monostate, LoadListingEnum>> loadListing;
    heap_object<std::variant<std::monostate, CommandLineBinding>> inputBinding;
    virtual ~CommandInputRecordField() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct CommandInputRecordSchema {
    heap_object<std::variant<std::monostate, std::vector<CommandInputRecordField>>> fields;
    heap_object<w3id_org::cwl::salad::RecordSchema_type_Record_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    heap_object<std::variant<std::monostate, CommandLineBinding>> inputBinding;
    virtual ~CommandInputRecordSchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct CommandInputEnumSchema {
    heap_object<std::variant<std::monostate, std::string>> name;
    heap_object<std::vector<std::string>> symbols;
    heap_object<w3id_org::cwl::salad::EnumSchema_type_Enum_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, CommandLineBinding>> inputBinding;
    virtual ~CommandInputEnumSchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct CommandInputArraySchema {
    heap_object<std::variant<CWLType, CommandInputRecordSchema, CommandInputEnumSchema, CommandInputArraySchema, std::string, std::vector<std::variant<CWLType, CommandInputRecordSchema, CommandInputEnumSchema, CommandInputArraySchema, std::string>>>> items;
    heap_object<w3id_org::cwl::salad::ArraySchema_type_Array_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    heap_object<std::variant<std::monostate, CommandLineBinding>> inputBinding;
    virtual ~CommandInputArraySchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct CommandOutputRecordField {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::string> name;
    heap_object<std::variant<CWLType, CommandOutputRecordSchema, CommandOutputEnumSchema, CommandOutputArraySchema, std::string, std::vector<std::variant<CWLType, CommandOutputRecordSchema, CommandOutputEnumSchema, CommandOutputArraySchema, std::string>>>> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, SecondaryFileSchema, std::vector<SecondaryFileSchema>>> secondaryFiles;
    heap_object<std::variant<std::monostate, bool>> streamable;
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> format;
    heap_object<std::variant<std::monostate, CommandOutputBinding>> outputBinding;
    virtual ~CommandOutputRecordField() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct CommandOutputRecordSchema {
    heap_object<std::variant<std::monostate, std::vector<CommandOutputRecordField>>> fields;
    heap_object<w3id_org::cwl::salad::RecordSchema_type_Record_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~CommandOutputRecordSchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct CommandOutputEnumSchema {
    heap_object<std::variant<std::monostate, std::string>> name;
    heap_object<std::vector<std::string>> symbols;
    heap_object<w3id_org::cwl::salad::EnumSchema_type_Enum_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    virtual ~CommandOutputEnumSchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct CommandOutputArraySchema {
    heap_object<std::variant<CWLType, CommandOutputRecordSchema, CommandOutputEnumSchema, CommandOutputArraySchema, std::string, std::vector<std::variant<CWLType, CommandOutputRecordSchema, CommandOutputEnumSchema, CommandOutputArraySchema, std::string>>>> items;
    heap_object<w3id_org::cwl::salad::ArraySchema_type_Array_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~CommandOutputArraySchema() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct CommandInputParameter
    : w3id_org::cwl::cwl::InputParameter {
    heap_object<std::variant<CWLType, stdin_, CommandInputRecordSchema, CommandInputEnumSchema, CommandInputArraySchema, std::string, std::vector<std::variant<CWLType, CommandInputRecordSchema, CommandInputEnumSchema, CommandInputArraySchema, std::string>>>> type;
    heap_object<std::variant<std::monostate, CommandLineBinding>> inputBinding;
    ~CommandInputParameter() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct CommandOutputParameter
    : w3id_org::cwl::cwl::OutputParameter {
    heap_object<std::variant<CWLType, stdout_, stderr_, CommandOutputRecordSchema, CommandOutputEnumSchema, CommandOutputArraySchema, std::string, std::vector<std::variant<CWLType, CommandOutputRecordSchema, CommandOutputEnumSchema, CommandOutputArraySchema, std::string>>>> type;
    heap_object<std::variant<std::monostate, CommandOutputBinding>> outputBinding;
    ~CommandOutputParameter() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct CommandLineTool {
    heap_object<std::variant<std::monostate, std::string>> id;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::vector<CommandInputParameter>> inputs;
    heap_object<std::vector<CommandOutputParameter>> outputs;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement>>>> requirements;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement, std::any>>>> hints;
    heap_object<std::variant<std::monostate, CWLVersion>> cwlVersion;
    heap_object<std::variant<std::monostate, std::vector<std::string>>> intent;
    heap_object<CommandLineTool_class_CommandLineTool_class> class_;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> baseCommand;
    heap_object<std::variant<std::monostate, std::vector<std::variant<std::string, cwl_expression_string, CommandLineBinding>>>> arguments;
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> stdin_;
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> stderr_;
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> stdout_;
    heap_object<std::variant<std::monostate, std::vector<int32_t>>> successCodes;
    heap_object<std::variant<std::monostate, std::vector<int32_t>>> temporaryFailCodes;
    heap_object<std::variant<std::monostate, std::vector<int32_t>>> permanentFailCodes;
    virtual ~CommandLineTool() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct DockerRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<DockerRequirement_class_DockerRequirement_class> class_;
    heap_object<std::variant<std::monostate, std::string>> dockerPull;
    heap_object<std::variant<std::monostate, std::string>> dockerLoad;
    heap_object<std::variant<std::monostate, std::string>> dockerFile;
    heap_object<std::variant<std::monostate, std::string>> dockerImport;
    heap_object<std::variant<std::monostate, std::string>> dockerImageId;
    heap_object<std::variant<std::monostate, std::string>> dockerOutputDirectory;
    ~DockerRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct SoftwareRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<SoftwareRequirement_class_SoftwareRequirement_class> class_;
    heap_object<std::vector<SoftwarePackage>> packages;
    ~SoftwareRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct SoftwarePackage {
    heap_object<std::string> package;
    heap_object<std::variant<std::monostate, std::vector<std::string>>> version;
    heap_object<std::variant<std::monostate, std::vector<std::string>>> specs;
    virtual ~SoftwarePackage() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct Dirent {
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> entryname;
    heap_object<std::variant<std::string, cwl_expression_string>> entry;
    heap_object<std::variant<std::monostate, bool>> writable;
    virtual ~Dirent() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct InitialWorkDirRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<InitialWorkDirRequirement_class_InitialWorkDirRequirement_class> class_;
    heap_object<std::variant<cwl_expression_string, std::vector<std::variant<std::monostate, Dirent, cwl_expression_string, File, Directory, std::vector<std::variant<File, Directory>>>>>> listing;
    ~InitialWorkDirRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct EnvVarRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<EnvVarRequirement_class_EnvVarRequirement_class> class_;
    heap_object<std::vector<EnvironmentDef>> envDef;
    ~EnvVarRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct ShellCommandRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<ShellCommandRequirement_class_ShellCommandRequirement_class> class_;
    ~ShellCommandRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct ResourceRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<ResourceRequirement_class_ResourceRequirement_class> class_;
    heap_object<std::variant<std::monostate, int32_t, int64_t, float, cwl_expression_string>> coresMin;
    heap_object<std::variant<std::monostate, int32_t, int64_t, float, cwl_expression_string>> coresMax;
    heap_object<std::variant<std::monostate, int32_t, int64_t, float, cwl_expression_string>> ramMin;
    heap_object<std::variant<std::monostate, int32_t, int64_t, float, cwl_expression_string>> ramMax;
    heap_object<std::variant<std::monostate, int32_t, int64_t, float, cwl_expression_string>> tmpdirMin;
    heap_object<std::variant<std::monostate, int32_t, int64_t, float, cwl_expression_string>> tmpdirMax;
    heap_object<std::variant<std::monostate, int32_t, int64_t, float, cwl_expression_string>> outdirMin;
    heap_object<std::variant<std::monostate, int32_t, int64_t, float, cwl_expression_string>> outdirMax;
    ~ResourceRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct WorkReuse
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<WorkReuse_class_WorkReuse_class> class_;
    heap_object<std::variant<bool, cwl_expression_string>> enableReuse;
    ~WorkReuse() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct NetworkAccess
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<NetworkAccess_class_NetworkAccess_class> class_;
    heap_object<std::variant<bool, cwl_expression_string>> networkAccess;
    ~NetworkAccess() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct InplaceUpdateRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<InplaceUpdateRequirement_class_InplaceUpdateRequirement_class> class_;
    heap_object<bool> inplaceUpdate;
    ~InplaceUpdateRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct ToolTimeLimit
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<ToolTimeLimit_class_ToolTimeLimit_class> class_;
    heap_object<std::variant<int32_t, int64_t, cwl_expression_string>> timelimit;
    ~ToolTimeLimit() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct ExpressionToolOutputParameter
    : w3id_org::cwl::cwl::OutputParameter {
    heap_object<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string, std::vector<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string>>>> type;
    ~ExpressionToolOutputParameter() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct WorkflowInputParameter
    : w3id_org::cwl::cwl::InputParameter {
    heap_object<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string, std::vector<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string>>>> type;
    heap_object<std::variant<std::monostate, InputBinding>> inputBinding;
    ~WorkflowInputParameter() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct ExpressionTool {
    heap_object<std::variant<std::monostate, std::string>> id;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::vector<WorkflowInputParameter>> inputs;
    heap_object<std::vector<ExpressionToolOutputParameter>> outputs;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement>>>> requirements;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement, std::any>>>> hints;
    heap_object<std::variant<std::monostate, CWLVersion>> cwlVersion;
    heap_object<std::variant<std::monostate, std::vector<std::string>>> intent;
    heap_object<ExpressionTool_class_ExpressionTool_class> class_;
    heap_object<cwl_expression_string> expression;
    virtual ~ExpressionTool() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct WorkflowOutputParameter
    : w3id_org::cwl::cwl::OutputParameter {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> outputSource;
    heap_object<std::variant<std::monostate, LinkMergeMethod>> linkMerge;
    heap_object<std::variant<std::monostate, PickValueMethod>> pickValue;
    heap_object<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string, std::vector<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string>>>> type;
    ~WorkflowOutputParameter() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct Sink {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> source;
    heap_object<std::variant<std::monostate, LinkMergeMethod>> linkMerge;
    heap_object<std::variant<std::monostate, PickValueMethod>> pickValue;
    virtual ~Sink() = 0;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct WorkflowStepInput
    : w3id_org::cwl::cwl::Identified
    , w3id_org::cwl::cwl::Sink
    , w3id_org::cwl::cwl::LoadContents
    , w3id_org::cwl::cwl::Labeled {
    heap_object<std::variant<std::monostate, File, Directory, std::any>> default_;
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> valueFrom;
    ~WorkflowStepInput() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct WorkflowStepOutput
    : w3id_org::cwl::cwl::Identified {
    ~WorkflowStepOutput() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct WorkflowStep
    : w3id_org::cwl::cwl::Identified
    , w3id_org::cwl::cwl::Labeled
    , w3id_org::cwl::salad::Documented {
    heap_object<std::vector<WorkflowStepInput>> in;
    heap_object<std::vector<std::variant<std::string, WorkflowStepOutput>>> out;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement>>>> requirements;
    heap_object<std::variant<std::monostate, std::vector<std::any>>> hints;
    heap_object<std::variant<std::string, CommandLineTool, ExpressionTool, Workflow, Operation>> run;
    heap_object<std::variant<std::monostate, cwl_expression_string>> when;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> scatter;
    heap_object<std::variant<std::monostate, ScatterMethod>> scatterMethod;
    ~WorkflowStep() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct Workflow {
    heap_object<std::variant<std::monostate, std::string>> id;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::vector<WorkflowInputParameter>> inputs;
    heap_object<std::vector<WorkflowOutputParameter>> outputs;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement>>>> requirements;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement, std::any>>>> hints;
    heap_object<std::variant<std::monostate, CWLVersion>> cwlVersion;
    heap_object<std::variant<std::monostate, std::vector<std::string>>> intent;
    heap_object<Workflow_class_Workflow_class> class_;
    heap_object<std::vector<WorkflowStep>> steps;
    virtual ~Workflow() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl::cwl {
struct SubworkflowFeatureRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class> class_;
    ~SubworkflowFeatureRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct ScatterFeatureRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<ScatterFeatureRequirement_class_ScatterFeatureRequirement_class> class_;
    ~ScatterFeatureRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct MultipleInputFeatureRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class> class_;
    ~MultipleInputFeatureRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct StepInputExpressionRequirement
    : w3id_org::cwl::cwl::ProcessRequirement {
    heap_object<StepInputExpressionRequirement_class_StepInputExpressionRequirement_class> class_;
    ~StepInputExpressionRequirement() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct OperationInputParameter
    : w3id_org::cwl::cwl::InputParameter {
    heap_object<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string, std::vector<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string>>>> type;
    ~OperationInputParameter() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct OperationOutputParameter
    : w3id_org::cwl::cwl::OutputParameter {
    heap_object<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string, std::vector<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string>>>> type;
    ~OperationOutputParameter() override = default;
    auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace w3id_org::cwl::cwl {
struct Operation {
    heap_object<std::variant<std::monostate, std::string>> id;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::vector<OperationInputParameter>> inputs;
    heap_object<std::vector<OperationOutputParameter>> outputs;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement>>>> requirements;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement, std::any>>>> hints;
    heap_object<std::variant<std::monostate, CWLVersion>> cwlVersion;
    heap_object<std::variant<std::monostate, std::vector<std::string>>> intent;
    heap_object<Operation_class_Operation_class> class_;
    virtual ~Operation() = default;
    virtual auto toYaml([[maybe_unused]] w3id_org::cwl::store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace w3id_org::cwl {
template <typename T> heap_object<T>::~heap_object() = default;
}

inline w3id_org::cwl::salad::Documented::~Documented() = default;
inline auto w3id_org::cwl::salad::Documented::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("Documented");
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    return n;
}
inline void w3id_org::cwl::salad::Documented::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
}
inline auto w3id_org::cwl::salad::RecordField::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("RecordField");
    }
    n = mergeYaml(n, w3id_org::cwl::salad::Documented::toYaml(config));
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    return n;
}
inline void w3id_org::cwl::salad::RecordField::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::salad::Documented::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::salad::RecordField> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::salad::RecordField> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::salad::RecordField{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::salad::RecordSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("RecordSchema");
    }
    {
         auto member = toYaml(*fields, config);
         member = convertListToMap(member, "name", "type", config);
        addYamlField(n, "fields", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    return n;
}
inline void w3id_org::cwl::salad::RecordSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["fields"], "name", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *fields);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::salad::RecordSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::salad::RecordSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::salad::RecordSchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.fields)::value_t>::value) try {
            fromYaml(n["fields"], *res.fields);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::salad::EnumSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("EnumSchema");
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    {
         auto member = toYaml(*symbols, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "symbols", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    return n;
}
inline void w3id_org::cwl::salad::EnumSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
    {
        auto nodeAsList = convertMapToList(n["symbols"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *symbols);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::salad::EnumSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::salad::EnumSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::salad::EnumSchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.symbols)::value_t>::value) try {
            fromYaml(n["symbols"], *res.symbols);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::salad::ArraySchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ArraySchema");
    }
    {
         auto member = toYaml(*items, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "items", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    return n;
}
inline void w3id_org::cwl::salad::ArraySchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["items"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *items);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::salad::ArraySchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::salad::ArraySchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::salad::ArraySchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.items)::value_t>::value) try {
            fromYaml(n["items"], *res.items);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::File::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("File");
    }
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*location, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "location", member);
    }
    {
         auto member = toYaml(*path, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "path", member);
    }
    {
         auto member = toYaml(*basename, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "basename", member);
    }
    {
         auto member = toYaml(*dirname, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "dirname", member);
    }
    {
         auto member = toYaml(*nameroot, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "nameroot", member);
    }
    {
         auto member = toYaml(*nameext, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "nameext", member);
    }
    {
         auto member = toYaml(*checksum, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "checksum", member);
    }
    {
         auto member = toYaml(*size, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "size", member);
    }
    {
         auto member = toYaml(*secondaryFiles, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "secondaryFiles", member);
    }
    {
         auto member = toYaml(*format, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "format", member);
    }
    {
         auto member = toYaml(*contents, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "contents", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::File::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["location"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *location);
    }
    {
        auto nodeAsList = convertMapToList(n["path"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *path);
    }
    {
        auto nodeAsList = convertMapToList(n["basename"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *basename);
    }
    {
        auto nodeAsList = convertMapToList(n["dirname"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *dirname);
    }
    {
        auto nodeAsList = convertMapToList(n["nameroot"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *nameroot);
    }
    {
        auto nodeAsList = convertMapToList(n["nameext"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *nameext);
    }
    {
        auto nodeAsList = convertMapToList(n["checksum"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *checksum);
    }
    {
        auto nodeAsList = convertMapToList(n["size"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *size);
    }
    {
        auto nodeAsList = convertMapToList(n["secondaryFiles"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *secondaryFiles);
    }
    {
        auto nodeAsList = convertMapToList(n["format"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *format);
    }
    {
        auto nodeAsList = convertMapToList(n["contents"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *contents);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::File> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::File> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::File{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.location)::value_t>::value) try {
            fromYaml(n["location"], *res.location);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.path)::value_t>::value) try {
            fromYaml(n["path"], *res.path);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.basename)::value_t>::value) try {
            fromYaml(n["basename"], *res.basename);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.dirname)::value_t>::value) try {
            fromYaml(n["dirname"], *res.dirname);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.nameroot)::value_t>::value) try {
            fromYaml(n["nameroot"], *res.nameroot);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.nameext)::value_t>::value) try {
            fromYaml(n["nameext"], *res.nameext);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.checksum)::value_t>::value) try {
            fromYaml(n["checksum"], *res.checksum);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.size)::value_t>::value) try {
            fromYaml(n["size"], *res.size);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.secondaryFiles)::value_t>::value) try {
            fromYaml(n["secondaryFiles"], *res.secondaryFiles);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.format)::value_t>::value) try {
            fromYaml(n["format"], *res.format);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.contents)::value_t>::value) try {
            fromYaml(n["contents"], *res.contents);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::Directory::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("Directory");
    }
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*location, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "location", member);
    }
    {
         auto member = toYaml(*path, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "path", member);
    }
    {
         auto member = toYaml(*basename, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "basename", member);
    }
    {
         auto member = toYaml(*listing, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "listing", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::Directory::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["location"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *location);
    }
    {
        auto nodeAsList = convertMapToList(n["path"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *path);
    }
    {
        auto nodeAsList = convertMapToList(n["basename"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *basename);
    }
    {
        auto nodeAsList = convertMapToList(n["listing"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *listing);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::Directory> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::Directory> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::Directory{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.location)::value_t>::value) try {
            fromYaml(n["location"], *res.location);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.path)::value_t>::value) try {
            fromYaml(n["path"], *res.path);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.basename)::value_t>::value) try {
            fromYaml(n["basename"], *res.basename);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.listing)::value_t>::value) try {
            fromYaml(n["listing"], *res.listing);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline w3id_org::cwl::cwl::Labeled::~Labeled() = default;
inline auto w3id_org::cwl::cwl::Labeled::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("Labeled");
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::Labeled::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
}
inline w3id_org::cwl::cwl::Identified::~Identified() = default;
inline auto w3id_org::cwl::cwl::Identified::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("Identified");
    }
    {
         auto member = toYaml(*id, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "id", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::Identified::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["id"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *id);
    }
}
inline w3id_org::cwl::cwl::LoadContents::~LoadContents() = default;
inline auto w3id_org::cwl::cwl::LoadContents::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("LoadContents");
    }
    {
         auto member = toYaml(*loadContents, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "loadContents", member);
    }
    {
         auto member = toYaml(*loadListing, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "loadListing", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::LoadContents::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["loadContents"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *loadContents);
    }
    {
        auto nodeAsList = convertMapToList(n["loadListing"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *loadListing);
    }
}
inline w3id_org::cwl::cwl::FieldBase::~FieldBase() = default;
inline auto w3id_org::cwl::cwl::FieldBase::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("FieldBase");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::Labeled::toYaml(config));
    {
         auto member = toYaml(*secondaryFiles, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "secondaryFiles", member);
    }
    {
         auto member = toYaml(*streamable, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "streamable", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::FieldBase::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::Labeled::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["secondaryFiles"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *secondaryFiles);
    }
    {
        auto nodeAsList = convertMapToList(n["streamable"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *streamable);
    }
}
inline w3id_org::cwl::cwl::InputFormat::~InputFormat() = default;
inline auto w3id_org::cwl::cwl::InputFormat::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InputFormat");
    }
    {
         auto member = toYaml(*format, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "format", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::InputFormat::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["format"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *format);
    }
}
inline w3id_org::cwl::cwl::OutputFormat::~OutputFormat() = default;
inline auto w3id_org::cwl::cwl::OutputFormat::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OutputFormat");
    }
    {
         auto member = toYaml(*format, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "format", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::OutputFormat::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["format"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *format);
    }
}
inline w3id_org::cwl::cwl::Parameter::~Parameter() = default;
inline auto w3id_org::cwl::cwl::Parameter::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("Parameter");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::FieldBase::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::salad::Documented::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::cwl::Identified::toYaml(config));
    return n;
}
inline void w3id_org::cwl::cwl::Parameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::FieldBase::fromYaml(n);
    w3id_org::cwl::salad::Documented::fromYaml(n);
    w3id_org::cwl::cwl::Identified::fromYaml(n);
}
inline auto w3id_org::cwl::cwl::InputBinding::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InputBinding");
    }
    {
         auto member = toYaml(*loadContents, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "loadContents", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::InputBinding::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["loadContents"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *loadContents);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::InputBinding> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::InputBinding> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::InputBinding{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.loadContents)::value_t>::value) try {
            fromYaml(n["loadContents"], *res.loadContents);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline w3id_org::cwl::cwl::IOSchema::~IOSchema() = default;
inline auto w3id_org::cwl::cwl::IOSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("IOSchema");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::Labeled::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::salad::Documented::toYaml(config));
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::IOSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::Labeled::fromYaml(n);
    w3id_org::cwl::salad::Documented::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
}
inline w3id_org::cwl::cwl::InputSchema::~InputSchema() = default;
inline auto w3id_org::cwl::cwl::InputSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InputSchema");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::IOSchema::toYaml(config));
    return n;
}
inline void w3id_org::cwl::cwl::InputSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::IOSchema::fromYaml(n);
}
inline w3id_org::cwl::cwl::OutputSchema::~OutputSchema() = default;
inline auto w3id_org::cwl::cwl::OutputSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OutputSchema");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::IOSchema::toYaml(config));
    return n;
}
inline void w3id_org::cwl::cwl::OutputSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::IOSchema::fromYaml(n);
}
inline auto w3id_org::cwl::cwl::InputRecordField::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InputRecordField");
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*secondaryFiles, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "secondaryFiles", member);
    }
    {
         auto member = toYaml(*streamable, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "streamable", member);
    }
    {
         auto member = toYaml(*format, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "format", member);
    }
    {
         auto member = toYaml(*loadContents, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "loadContents", member);
    }
    {
         auto member = toYaml(*loadListing, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "loadListing", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::InputRecordField::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["secondaryFiles"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *secondaryFiles);
    }
    {
        auto nodeAsList = convertMapToList(n["streamable"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *streamable);
    }
    {
        auto nodeAsList = convertMapToList(n["format"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *format);
    }
    {
        auto nodeAsList = convertMapToList(n["loadContents"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *loadContents);
    }
    {
        auto nodeAsList = convertMapToList(n["loadListing"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *loadListing);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::InputRecordField> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::InputRecordField> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::InputRecordField{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.secondaryFiles)::value_t>::value) try {
            fromYaml(n["secondaryFiles"], *res.secondaryFiles);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.streamable)::value_t>::value) try {
            fromYaml(n["streamable"], *res.streamable);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.format)::value_t>::value) try {
            fromYaml(n["format"], *res.format);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.loadContents)::value_t>::value) try {
            fromYaml(n["loadContents"], *res.loadContents);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.loadListing)::value_t>::value) try {
            fromYaml(n["loadListing"], *res.loadListing);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::InputRecordSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InputRecordSchema");
    }
    {
         auto member = toYaml(*fields, config);
         member = convertListToMap(member, "name", "type", config);
        addYamlField(n, "fields", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::InputRecordSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["fields"], "name", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *fields);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::InputRecordSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::InputRecordSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::InputRecordSchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.fields)::value_t>::value) try {
            fromYaml(n["fields"], *res.fields);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::InputEnumSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InputEnumSchema");
    }
    n = mergeYaml(n, w3id_org::cwl::salad::EnumSchema::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::cwl::InputSchema::toYaml(config));
    return n;
}
inline void w3id_org::cwl::cwl::InputEnumSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::salad::EnumSchema::fromYaml(n);
    w3id_org::cwl::cwl::InputSchema::fromYaml(n);
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::InputEnumSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::InputEnumSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::InputEnumSchema{};

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::InputArraySchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InputArraySchema");
    }
    {
         auto member = toYaml(*items, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "items", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::InputArraySchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["items"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *items);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::InputArraySchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::InputArraySchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::InputArraySchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.items)::value_t>::value) try {
            fromYaml(n["items"], *res.items);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::OutputRecordField::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OutputRecordField");
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*secondaryFiles, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "secondaryFiles", member);
    }
    {
         auto member = toYaml(*streamable, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "streamable", member);
    }
    {
         auto member = toYaml(*format, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "format", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::OutputRecordField::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["secondaryFiles"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *secondaryFiles);
    }
    {
        auto nodeAsList = convertMapToList(n["streamable"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *streamable);
    }
    {
        auto nodeAsList = convertMapToList(n["format"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *format);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::OutputRecordField> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::OutputRecordField> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::OutputRecordField{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.secondaryFiles)::value_t>::value) try {
            fromYaml(n["secondaryFiles"], *res.secondaryFiles);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.streamable)::value_t>::value) try {
            fromYaml(n["streamable"], *res.streamable);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.format)::value_t>::value) try {
            fromYaml(n["format"], *res.format);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::OutputRecordSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OutputRecordSchema");
    }
    {
         auto member = toYaml(*fields, config);
         member = convertListToMap(member, "name", "type", config);
        addYamlField(n, "fields", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::OutputRecordSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["fields"], "name", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *fields);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::OutputRecordSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::OutputRecordSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::OutputRecordSchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.fields)::value_t>::value) try {
            fromYaml(n["fields"], *res.fields);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::OutputEnumSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OutputEnumSchema");
    }
    n = mergeYaml(n, w3id_org::cwl::salad::EnumSchema::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::cwl::OutputSchema::toYaml(config));
    return n;
}
inline void w3id_org::cwl::cwl::OutputEnumSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::salad::EnumSchema::fromYaml(n);
    w3id_org::cwl::cwl::OutputSchema::fromYaml(n);
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::OutputEnumSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::OutputEnumSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::OutputEnumSchema{};

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::OutputArraySchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OutputArraySchema");
    }
    {
         auto member = toYaml(*items, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "items", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::OutputArraySchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["items"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *items);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::OutputArraySchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::OutputArraySchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::OutputArraySchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.items)::value_t>::value) try {
            fromYaml(n["items"], *res.items);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline w3id_org::cwl::cwl::InputParameter::~InputParameter() = default;
inline auto w3id_org::cwl::cwl::InputParameter::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InputParameter");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::Parameter::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::cwl::InputFormat::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::cwl::LoadContents::toYaml(config));
    {
         auto member = toYaml(*default_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "default", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::InputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::Parameter::fromYaml(n);
    w3id_org::cwl::cwl::InputFormat::fromYaml(n);
    w3id_org::cwl::cwl::LoadContents::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["default"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *default_);
    }
}
inline w3id_org::cwl::cwl::OutputParameter::~OutputParameter() = default;
inline auto w3id_org::cwl::cwl::OutputParameter::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OutputParameter");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::Parameter::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::cwl::OutputFormat::toYaml(config));
    return n;
}
inline void w3id_org::cwl::cwl::OutputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::Parameter::fromYaml(n);
    w3id_org::cwl::cwl::OutputFormat::fromYaml(n);
}
inline w3id_org::cwl::cwl::ProcessRequirement::~ProcessRequirement() = default;
inline auto w3id_org::cwl::cwl::ProcessRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ProcessRequirement");
    }
    return n;
}
inline void w3id_org::cwl::cwl::ProcessRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
}
inline w3id_org::cwl::cwl::Process::~Process() = default;
inline auto w3id_org::cwl::cwl::Process::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("Process");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::Identified::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::cwl::Labeled::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::salad::Documented::toYaml(config));
    {
         auto member = toYaml(*inputs, config);
         member = convertListToMap(member, "id", "type", config);
        addYamlField(n, "inputs", member);
    }
    {
         auto member = toYaml(*outputs, config);
         member = convertListToMap(member, "id", "type", config);
        addYamlField(n, "outputs", member);
    }
    {
         auto member = toYaml(*requirements, config);
         member = convertListToMap(member, "class", "", config);
        addYamlField(n, "requirements", member);
    }
    {
         auto member = toYaml(*hints, config);
         member = convertListToMap(member, "class", "", config);
        addYamlField(n, "hints", member);
    }
    {
         auto member = toYaml(*cwlVersion, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "cwlVersion", member);
    }
    {
         auto member = toYaml(*intent, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "intent", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::Process::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::Identified::fromYaml(n);
    w3id_org::cwl::cwl::Labeled::fromYaml(n);
    w3id_org::cwl::salad::Documented::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["inputs"], "id", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputs);
    }
    {
        auto nodeAsList = convertMapToList(n["outputs"], "id", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *outputs);
    }
    {
        auto nodeAsList = convertMapToList(n["requirements"], "class", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *requirements);
    }
    {
        auto nodeAsList = convertMapToList(n["hints"], "class", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *hints);
    }
    {
        auto nodeAsList = convertMapToList(n["cwlVersion"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *cwlVersion);
    }
    {
        auto nodeAsList = convertMapToList(n["intent"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *intent);
    }
}
inline auto w3id_org::cwl::cwl::InlineJavascriptRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InlineJavascriptRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*expressionLib, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "expressionLib", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::InlineJavascriptRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["expressionLib"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *expressionLib);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::InlineJavascriptRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::InlineJavascriptRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::InlineJavascriptRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.expressionLib)::value_t>::value) try {
            fromYaml(n["expressionLib"], *res.expressionLib);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline w3id_org::cwl::cwl::CommandInputSchema::~CommandInputSchema() = default;
inline auto w3id_org::cwl::cwl::CommandInputSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandInputSchema");
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandInputSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
}
inline auto w3id_org::cwl::cwl::SchemaDefRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("SchemaDefRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*types, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "types", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::SchemaDefRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["types"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *types);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::SchemaDefRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::SchemaDefRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::SchemaDefRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.types)::value_t>::value) try {
            fromYaml(n["types"], *res.types);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::SecondaryFileSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("SecondaryFileSchema");
    }
    {
         auto member = toYaml(*pattern, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "pattern", member);
    }
    {
         auto member = toYaml(*required, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "required", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::SecondaryFileSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["pattern"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *pattern);
    }
    {
        auto nodeAsList = convertMapToList(n["required"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *required);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::SecondaryFileSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::SecondaryFileSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::SecondaryFileSchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.pattern)::value_t>::value) try {
            fromYaml(n["pattern"], *res.pattern);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.required)::value_t>::value) try {
            fromYaml(n["required"], *res.required);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::LoadListingRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("LoadListingRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*loadListing, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "loadListing", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::LoadListingRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["loadListing"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *loadListing);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::LoadListingRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::LoadListingRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::LoadListingRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.loadListing)::value_t>::value) try {
            fromYaml(n["loadListing"], *res.loadListing);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::EnvironmentDef::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("EnvironmentDef");
    }
    {
         auto member = toYaml(*envName, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "envName", member);
    }
    {
         auto member = toYaml(*envValue, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "envValue", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::EnvironmentDef::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["envName"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *envName);
    }
    {
        auto nodeAsList = convertMapToList(n["envValue"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *envValue);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::EnvironmentDef> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::EnvironmentDef> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::EnvironmentDef{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.envName)::value_t>::value) try {
            fromYaml(n["envName"], *res.envName);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.envValue)::value_t>::value) try {
            fromYaml(n["envValue"], *res.envValue);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandLineBinding::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandLineBinding");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::InputBinding::toYaml(config));
    {
         auto member = toYaml(*position, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "position", member);
    }
    {
         auto member = toYaml(*prefix, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "prefix", member);
    }
    {
         auto member = toYaml(*separate, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "separate", member);
    }
    {
         auto member = toYaml(*itemSeparator, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "itemSeparator", member);
    }
    {
         auto member = toYaml(*valueFrom, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "valueFrom", member);
    }
    {
         auto member = toYaml(*shellQuote, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "shellQuote", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandLineBinding::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::InputBinding::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["position"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *position);
    }
    {
        auto nodeAsList = convertMapToList(n["prefix"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *prefix);
    }
    {
        auto nodeAsList = convertMapToList(n["separate"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *separate);
    }
    {
        auto nodeAsList = convertMapToList(n["itemSeparator"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *itemSeparator);
    }
    {
        auto nodeAsList = convertMapToList(n["valueFrom"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *valueFrom);
    }
    {
        auto nodeAsList = convertMapToList(n["shellQuote"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *shellQuote);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandLineBinding> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandLineBinding> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandLineBinding{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.position)::value_t>::value) try {
            fromYaml(n["position"], *res.position);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.prefix)::value_t>::value) try {
            fromYaml(n["prefix"], *res.prefix);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.separate)::value_t>::value) try {
            fromYaml(n["separate"], *res.separate);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.itemSeparator)::value_t>::value) try {
            fromYaml(n["itemSeparator"], *res.itemSeparator);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.valueFrom)::value_t>::value) try {
            fromYaml(n["valueFrom"], *res.valueFrom);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.shellQuote)::value_t>::value) try {
            fromYaml(n["shellQuote"], *res.shellQuote);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandOutputBinding::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandOutputBinding");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::LoadContents::toYaml(config));
    {
         auto member = toYaml(*glob, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "glob", member);
    }
    {
         auto member = toYaml(*outputEval, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "outputEval", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandOutputBinding::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::LoadContents::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["glob"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *glob);
    }
    {
        auto nodeAsList = convertMapToList(n["outputEval"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *outputEval);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandOutputBinding> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandOutputBinding> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandOutputBinding{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.glob)::value_t>::value) try {
            fromYaml(n["glob"], *res.glob);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.outputEval)::value_t>::value) try {
            fromYaml(n["outputEval"], *res.outputEval);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandLineBindable::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandLineBindable");
    }
    {
         auto member = toYaml(*inputBinding, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "inputBinding", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandLineBindable::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["inputBinding"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputBinding);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandLineBindable> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandLineBindable> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandLineBindable{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandInputRecordField::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandInputRecordField");
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*secondaryFiles, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "secondaryFiles", member);
    }
    {
         auto member = toYaml(*streamable, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "streamable", member);
    }
    {
         auto member = toYaml(*format, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "format", member);
    }
    {
         auto member = toYaml(*loadContents, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "loadContents", member);
    }
    {
         auto member = toYaml(*loadListing, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "loadListing", member);
    }
    {
         auto member = toYaml(*inputBinding, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "inputBinding", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandInputRecordField::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["secondaryFiles"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *secondaryFiles);
    }
    {
        auto nodeAsList = convertMapToList(n["streamable"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *streamable);
    }
    {
        auto nodeAsList = convertMapToList(n["format"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *format);
    }
    {
        auto nodeAsList = convertMapToList(n["loadContents"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *loadContents);
    }
    {
        auto nodeAsList = convertMapToList(n["loadListing"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *loadListing);
    }
    {
        auto nodeAsList = convertMapToList(n["inputBinding"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputBinding);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandInputRecordField> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandInputRecordField> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandInputRecordField{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.secondaryFiles)::value_t>::value) try {
            fromYaml(n["secondaryFiles"], *res.secondaryFiles);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.streamable)::value_t>::value) try {
            fromYaml(n["streamable"], *res.streamable);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.format)::value_t>::value) try {
            fromYaml(n["format"], *res.format);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.loadContents)::value_t>::value) try {
            fromYaml(n["loadContents"], *res.loadContents);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.loadListing)::value_t>::value) try {
            fromYaml(n["loadListing"], *res.loadListing);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandInputRecordSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandInputRecordSchema");
    }
    {
         auto member = toYaml(*fields, config);
         member = convertListToMap(member, "name", "type", config);
        addYamlField(n, "fields", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    {
         auto member = toYaml(*inputBinding, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "inputBinding", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandInputRecordSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["fields"], "name", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *fields);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
    {
        auto nodeAsList = convertMapToList(n["inputBinding"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputBinding);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandInputRecordSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandInputRecordSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandInputRecordSchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.fields)::value_t>::value) try {
            fromYaml(n["fields"], *res.fields);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandInputEnumSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandInputEnumSchema");
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    {
         auto member = toYaml(*symbols, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "symbols", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*inputBinding, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "inputBinding", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandInputEnumSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
    {
        auto nodeAsList = convertMapToList(n["symbols"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *symbols);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["inputBinding"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputBinding);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandInputEnumSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandInputEnumSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandInputEnumSchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.symbols)::value_t>::value) try {
            fromYaml(n["symbols"], *res.symbols);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandInputArraySchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandInputArraySchema");
    }
    {
         auto member = toYaml(*items, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "items", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    {
         auto member = toYaml(*inputBinding, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "inputBinding", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandInputArraySchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["items"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *items);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
    {
        auto nodeAsList = convertMapToList(n["inputBinding"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputBinding);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandInputArraySchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandInputArraySchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandInputArraySchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.items)::value_t>::value) try {
            fromYaml(n["items"], *res.items);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandOutputRecordField::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandOutputRecordField");
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*secondaryFiles, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "secondaryFiles", member);
    }
    {
         auto member = toYaml(*streamable, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "streamable", member);
    }
    {
         auto member = toYaml(*format, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "format", member);
    }
    {
         auto member = toYaml(*outputBinding, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "outputBinding", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandOutputRecordField::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["secondaryFiles"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *secondaryFiles);
    }
    {
        auto nodeAsList = convertMapToList(n["streamable"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *streamable);
    }
    {
        auto nodeAsList = convertMapToList(n["format"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *format);
    }
    {
        auto nodeAsList = convertMapToList(n["outputBinding"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *outputBinding);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandOutputRecordField> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandOutputRecordField> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandOutputRecordField{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.secondaryFiles)::value_t>::value) try {
            fromYaml(n["secondaryFiles"], *res.secondaryFiles);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.streamable)::value_t>::value) try {
            fromYaml(n["streamable"], *res.streamable);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.format)::value_t>::value) try {
            fromYaml(n["format"], *res.format);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.outputBinding)::value_t>::value) try {
            fromYaml(n["outputBinding"], *res.outputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandOutputRecordSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandOutputRecordSchema");
    }
    {
         auto member = toYaml(*fields, config);
         member = convertListToMap(member, "name", "type", config);
        addYamlField(n, "fields", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandOutputRecordSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["fields"], "name", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *fields);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandOutputRecordSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandOutputRecordSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandOutputRecordSchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.fields)::value_t>::value) try {
            fromYaml(n["fields"], *res.fields);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandOutputEnumSchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandOutputEnumSchema");
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    {
         auto member = toYaml(*symbols, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "symbols", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandOutputEnumSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
    {
        auto nodeAsList = convertMapToList(n["symbols"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *symbols);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandOutputEnumSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandOutputEnumSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandOutputEnumSchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.symbols)::value_t>::value) try {
            fromYaml(n["symbols"], *res.symbols);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandOutputArraySchema::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandOutputArraySchema");
    }
    {
         auto member = toYaml(*items, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "items", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandOutputArraySchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["items"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *items);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandOutputArraySchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandOutputArraySchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandOutputArraySchema{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.items)::value_t>::value) try {
            fromYaml(n["items"], *res.items);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandInputParameter::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandInputParameter");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::InputParameter::toYaml(config));
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*inputBinding, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "inputBinding", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandInputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::InputParameter::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["inputBinding"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputBinding);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandInputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandInputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandInputParameter{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandOutputParameter::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandOutputParameter");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::OutputParameter::toYaml(config));
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*outputBinding, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "outputBinding", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandOutputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::OutputParameter::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["outputBinding"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *outputBinding);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandOutputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandOutputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandOutputParameter{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.outputBinding)::value_t>::value) try {
            fromYaml(n["outputBinding"], *res.outputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::CommandLineTool::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandLineTool");
    }
    {
         auto member = toYaml(*id, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "id", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*inputs, config);
         member = convertListToMap(member, "id", "type", config);
        addYamlField(n, "inputs", member);
    }
    {
         auto member = toYaml(*outputs, config);
         member = convertListToMap(member, "id", "type", config);
        addYamlField(n, "outputs", member);
    }
    {
         auto member = toYaml(*requirements, config);
         member = convertListToMap(member, "class", "", config);
        addYamlField(n, "requirements", member);
    }
    {
         auto member = toYaml(*hints, config);
         member = convertListToMap(member, "class", "", config);
        addYamlField(n, "hints", member);
    }
    {
         auto member = toYaml(*cwlVersion, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "cwlVersion", member);
    }
    {
         auto member = toYaml(*intent, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "intent", member);
    }
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*baseCommand, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "baseCommand", member);
    }
    {
         auto member = toYaml(*arguments, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "arguments", member);
    }
    {
         auto member = toYaml(*stdin_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "stdin", member);
    }
    {
         auto member = toYaml(*stderr_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "stderr", member);
    }
    {
         auto member = toYaml(*stdout_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "stdout", member);
    }
    {
         auto member = toYaml(*successCodes, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "successCodes", member);
    }
    {
         auto member = toYaml(*temporaryFailCodes, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "temporaryFailCodes", member);
    }
    {
         auto member = toYaml(*permanentFailCodes, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "permanentFailCodes", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::CommandLineTool::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["id"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *id);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["inputs"], "id", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputs);
    }
    {
        auto nodeAsList = convertMapToList(n["outputs"], "id", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *outputs);
    }
    {
        auto nodeAsList = convertMapToList(n["requirements"], "class", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *requirements);
    }
    {
        auto nodeAsList = convertMapToList(n["hints"], "class", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *hints);
    }
    {
        auto nodeAsList = convertMapToList(n["cwlVersion"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *cwlVersion);
    }
    {
        auto nodeAsList = convertMapToList(n["intent"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *intent);
    }
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["baseCommand"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *baseCommand);
    }
    {
        auto nodeAsList = convertMapToList(n["arguments"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *arguments);
    }
    {
        auto nodeAsList = convertMapToList(n["stdin"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *stdin_);
    }
    {
        auto nodeAsList = convertMapToList(n["stderr"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *stderr_);
    }
    {
        auto nodeAsList = convertMapToList(n["stdout"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *stdout_);
    }
    {
        auto nodeAsList = convertMapToList(n["successCodes"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *successCodes);
    }
    {
        auto nodeAsList = convertMapToList(n["temporaryFailCodes"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *temporaryFailCodes);
    }
    {
        auto nodeAsList = convertMapToList(n["permanentFailCodes"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *permanentFailCodes);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::CommandLineTool> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::CommandLineTool> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::CommandLineTool{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.id)::value_t>::value) try {
            fromYaml(n["id"], *res.id);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.inputs)::value_t>::value) try {
            fromYaml(n["inputs"], *res.inputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.outputs)::value_t>::value) try {
            fromYaml(n["outputs"], *res.outputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.requirements)::value_t>::value) try {
            fromYaml(n["requirements"], *res.requirements);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.hints)::value_t>::value) try {
            fromYaml(n["hints"], *res.hints);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.cwlVersion)::value_t>::value) try {
            fromYaml(n["cwlVersion"], *res.cwlVersion);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.intent)::value_t>::value) try {
            fromYaml(n["intent"], *res.intent);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.baseCommand)::value_t>::value) try {
            fromYaml(n["baseCommand"], *res.baseCommand);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.arguments)::value_t>::value) try {
            fromYaml(n["arguments"], *res.arguments);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.stdin_)::value_t>::value) try {
            fromYaml(n["stdin"], *res.stdin_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.stderr_)::value_t>::value) try {
            fromYaml(n["stderr"], *res.stderr_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.stdout_)::value_t>::value) try {
            fromYaml(n["stdout"], *res.stdout_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.successCodes)::value_t>::value) try {
            fromYaml(n["successCodes"], *res.successCodes);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.temporaryFailCodes)::value_t>::value) try {
            fromYaml(n["temporaryFailCodes"], *res.temporaryFailCodes);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.permanentFailCodes)::value_t>::value) try {
            fromYaml(n["permanentFailCodes"], *res.permanentFailCodes);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::DockerRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("DockerRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*dockerPull, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "dockerPull", member);
    }
    {
         auto member = toYaml(*dockerLoad, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "dockerLoad", member);
    }
    {
         auto member = toYaml(*dockerFile, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "dockerFile", member);
    }
    {
         auto member = toYaml(*dockerImport, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "dockerImport", member);
    }
    {
         auto member = toYaml(*dockerImageId, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "dockerImageId", member);
    }
    {
         auto member = toYaml(*dockerOutputDirectory, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "dockerOutputDirectory", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::DockerRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["dockerPull"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *dockerPull);
    }
    {
        auto nodeAsList = convertMapToList(n["dockerLoad"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *dockerLoad);
    }
    {
        auto nodeAsList = convertMapToList(n["dockerFile"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *dockerFile);
    }
    {
        auto nodeAsList = convertMapToList(n["dockerImport"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *dockerImport);
    }
    {
        auto nodeAsList = convertMapToList(n["dockerImageId"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *dockerImageId);
    }
    {
        auto nodeAsList = convertMapToList(n["dockerOutputDirectory"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *dockerOutputDirectory);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::DockerRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::DockerRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::DockerRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.dockerPull)::value_t>::value) try {
            fromYaml(n["dockerPull"], *res.dockerPull);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.dockerLoad)::value_t>::value) try {
            fromYaml(n["dockerLoad"], *res.dockerLoad);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.dockerFile)::value_t>::value) try {
            fromYaml(n["dockerFile"], *res.dockerFile);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.dockerImport)::value_t>::value) try {
            fromYaml(n["dockerImport"], *res.dockerImport);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.dockerImageId)::value_t>::value) try {
            fromYaml(n["dockerImageId"], *res.dockerImageId);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.dockerOutputDirectory)::value_t>::value) try {
            fromYaml(n["dockerOutputDirectory"], *res.dockerOutputDirectory);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::SoftwareRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("SoftwareRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*packages, config);
         member = convertListToMap(member, "package", "specs", config);
        addYamlField(n, "packages", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::SoftwareRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["packages"], "package", "specs");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *packages);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::SoftwareRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::SoftwareRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::SoftwareRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.packages)::value_t>::value) try {
            fromYaml(n["packages"], *res.packages);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::SoftwarePackage::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("SoftwarePackage");
    }
    {
         auto member = toYaml(*package, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "package", member);
    }
    {
         auto member = toYaml(*version, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "version", member);
    }
    {
         auto member = toYaml(*specs, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "specs", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::SoftwarePackage::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["package"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *package);
    }
    {
        auto nodeAsList = convertMapToList(n["version"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *version);
    }
    {
        auto nodeAsList = convertMapToList(n["specs"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *specs);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::SoftwarePackage> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::SoftwarePackage> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::SoftwarePackage{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.package)::value_t>::value) try {
            fromYaml(n["package"], *res.package);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.version)::value_t>::value) try {
            fromYaml(n["version"], *res.version);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.specs)::value_t>::value) try {
            fromYaml(n["specs"], *res.specs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::Dirent::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("Dirent");
    }
    {
         auto member = toYaml(*entryname, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "entryname", member);
    }
    {
         auto member = toYaml(*entry, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "entry", member);
    }
    {
         auto member = toYaml(*writable, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "writable", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::Dirent::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["entryname"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *entryname);
    }
    {
        auto nodeAsList = convertMapToList(n["entry"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *entry);
    }
    {
        auto nodeAsList = convertMapToList(n["writable"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *writable);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::Dirent> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::Dirent> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::Dirent{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.entryname)::value_t>::value) try {
            fromYaml(n["entryname"], *res.entryname);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.entry)::value_t>::value) try {
            fromYaml(n["entry"], *res.entry);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.writable)::value_t>::value) try {
            fromYaml(n["writable"], *res.writable);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::InitialWorkDirRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InitialWorkDirRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*listing, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "listing", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::InitialWorkDirRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["listing"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *listing);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::InitialWorkDirRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::InitialWorkDirRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::InitialWorkDirRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.listing)::value_t>::value) try {
            fromYaml(n["listing"], *res.listing);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::EnvVarRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("EnvVarRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*envDef, config);
         member = convertListToMap(member, "envName", "envValue", config);
        addYamlField(n, "envDef", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::EnvVarRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["envDef"], "envName", "envValue");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *envDef);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::EnvVarRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::EnvVarRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::EnvVarRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.envDef)::value_t>::value) try {
            fromYaml(n["envDef"], *res.envDef);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::ShellCommandRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ShellCommandRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::ShellCommandRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::ShellCommandRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::ShellCommandRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::ShellCommandRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::ResourceRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ResourceRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*coresMin, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "coresMin", member);
    }
    {
         auto member = toYaml(*coresMax, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "coresMax", member);
    }
    {
         auto member = toYaml(*ramMin, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "ramMin", member);
    }
    {
         auto member = toYaml(*ramMax, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "ramMax", member);
    }
    {
         auto member = toYaml(*tmpdirMin, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "tmpdirMin", member);
    }
    {
         auto member = toYaml(*tmpdirMax, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "tmpdirMax", member);
    }
    {
         auto member = toYaml(*outdirMin, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "outdirMin", member);
    }
    {
         auto member = toYaml(*outdirMax, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "outdirMax", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::ResourceRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["coresMin"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *coresMin);
    }
    {
        auto nodeAsList = convertMapToList(n["coresMax"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *coresMax);
    }
    {
        auto nodeAsList = convertMapToList(n["ramMin"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *ramMin);
    }
    {
        auto nodeAsList = convertMapToList(n["ramMax"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *ramMax);
    }
    {
        auto nodeAsList = convertMapToList(n["tmpdirMin"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *tmpdirMin);
    }
    {
        auto nodeAsList = convertMapToList(n["tmpdirMax"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *tmpdirMax);
    }
    {
        auto nodeAsList = convertMapToList(n["outdirMin"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *outdirMin);
    }
    {
        auto nodeAsList = convertMapToList(n["outdirMax"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *outdirMax);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::ResourceRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::ResourceRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::ResourceRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.coresMin)::value_t>::value) try {
            fromYaml(n["coresMin"], *res.coresMin);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.coresMax)::value_t>::value) try {
            fromYaml(n["coresMax"], *res.coresMax);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.ramMin)::value_t>::value) try {
            fromYaml(n["ramMin"], *res.ramMin);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.ramMax)::value_t>::value) try {
            fromYaml(n["ramMax"], *res.ramMax);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.tmpdirMin)::value_t>::value) try {
            fromYaml(n["tmpdirMin"], *res.tmpdirMin);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.tmpdirMax)::value_t>::value) try {
            fromYaml(n["tmpdirMax"], *res.tmpdirMax);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.outdirMin)::value_t>::value) try {
            fromYaml(n["outdirMin"], *res.outdirMin);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.outdirMax)::value_t>::value) try {
            fromYaml(n["outdirMax"], *res.outdirMax);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::WorkReuse::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("WorkReuse");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*enableReuse, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "enableReuse", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::WorkReuse::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["enableReuse"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *enableReuse);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::WorkReuse> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::WorkReuse> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::WorkReuse{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.enableReuse)::value_t>::value) try {
            fromYaml(n["enableReuse"], *res.enableReuse);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::NetworkAccess::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("NetworkAccess");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*networkAccess, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "networkAccess", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::NetworkAccess::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["networkAccess"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *networkAccess);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::NetworkAccess> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::NetworkAccess> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::NetworkAccess{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.networkAccess)::value_t>::value) try {
            fromYaml(n["networkAccess"], *res.networkAccess);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::InplaceUpdateRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InplaceUpdateRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*inplaceUpdate, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "inplaceUpdate", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::InplaceUpdateRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["inplaceUpdate"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inplaceUpdate);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::InplaceUpdateRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::InplaceUpdateRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::InplaceUpdateRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.inplaceUpdate)::value_t>::value) try {
            fromYaml(n["inplaceUpdate"], *res.inplaceUpdate);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::ToolTimeLimit::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ToolTimeLimit");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*timelimit, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "timelimit", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::ToolTimeLimit::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["timelimit"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *timelimit);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::ToolTimeLimit> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::ToolTimeLimit> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::ToolTimeLimit{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.timelimit)::value_t>::value) try {
            fromYaml(n["timelimit"], *res.timelimit);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::ExpressionToolOutputParameter::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ExpressionToolOutputParameter");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::OutputParameter::toYaml(config));
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::ExpressionToolOutputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::OutputParameter::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::ExpressionToolOutputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::ExpressionToolOutputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::ExpressionToolOutputParameter{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::WorkflowInputParameter::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("WorkflowInputParameter");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::InputParameter::toYaml(config));
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    {
         auto member = toYaml(*inputBinding, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "inputBinding", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::WorkflowInputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::InputParameter::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
    {
        auto nodeAsList = convertMapToList(n["inputBinding"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputBinding);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::WorkflowInputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::WorkflowInputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::WorkflowInputParameter{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::ExpressionTool::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ExpressionTool");
    }
    {
         auto member = toYaml(*id, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "id", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*inputs, config);
         member = convertListToMap(member, "id", "type", config);
        addYamlField(n, "inputs", member);
    }
    {
         auto member = toYaml(*outputs, config);
         member = convertListToMap(member, "id", "type", config);
        addYamlField(n, "outputs", member);
    }
    {
         auto member = toYaml(*requirements, config);
         member = convertListToMap(member, "class", "", config);
        addYamlField(n, "requirements", member);
    }
    {
         auto member = toYaml(*hints, config);
         member = convertListToMap(member, "class", "", config);
        addYamlField(n, "hints", member);
    }
    {
         auto member = toYaml(*cwlVersion, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "cwlVersion", member);
    }
    {
         auto member = toYaml(*intent, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "intent", member);
    }
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*expression, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "expression", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::ExpressionTool::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["id"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *id);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["inputs"], "id", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputs);
    }
    {
        auto nodeAsList = convertMapToList(n["outputs"], "id", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *outputs);
    }
    {
        auto nodeAsList = convertMapToList(n["requirements"], "class", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *requirements);
    }
    {
        auto nodeAsList = convertMapToList(n["hints"], "class", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *hints);
    }
    {
        auto nodeAsList = convertMapToList(n["cwlVersion"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *cwlVersion);
    }
    {
        auto nodeAsList = convertMapToList(n["intent"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *intent);
    }
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["expression"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *expression);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::ExpressionTool> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::ExpressionTool> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::ExpressionTool{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.id)::value_t>::value) try {
            fromYaml(n["id"], *res.id);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.inputs)::value_t>::value) try {
            fromYaml(n["inputs"], *res.inputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.outputs)::value_t>::value) try {
            fromYaml(n["outputs"], *res.outputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.requirements)::value_t>::value) try {
            fromYaml(n["requirements"], *res.requirements);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.hints)::value_t>::value) try {
            fromYaml(n["hints"], *res.hints);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.cwlVersion)::value_t>::value) try {
            fromYaml(n["cwlVersion"], *res.cwlVersion);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.intent)::value_t>::value) try {
            fromYaml(n["intent"], *res.intent);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.expression)::value_t>::value) try {
            fromYaml(n["expression"], *res.expression);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::WorkflowOutputParameter::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("WorkflowOutputParameter");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::OutputParameter::toYaml(config));
    {
         auto member = toYaml(*outputSource, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "outputSource", member);
    }
    {
         auto member = toYaml(*linkMerge, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "linkMerge", member);
    }
    {
         auto member = toYaml(*pickValue, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "pickValue", member);
    }
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::WorkflowOutputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::OutputParameter::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["outputSource"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *outputSource);
    }
    {
        auto nodeAsList = convertMapToList(n["linkMerge"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *linkMerge);
    }
    {
        auto nodeAsList = convertMapToList(n["pickValue"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *pickValue);
    }
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::WorkflowOutputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::WorkflowOutputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::WorkflowOutputParameter{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.outputSource)::value_t>::value) try {
            fromYaml(n["outputSource"], *res.outputSource);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.linkMerge)::value_t>::value) try {
            fromYaml(n["linkMerge"], *res.linkMerge);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.pickValue)::value_t>::value) try {
            fromYaml(n["pickValue"], *res.pickValue);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline w3id_org::cwl::cwl::Sink::~Sink() = default;
inline auto w3id_org::cwl::cwl::Sink::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("Sink");
    }
    {
         auto member = toYaml(*source, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "source", member);
    }
    {
         auto member = toYaml(*linkMerge, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "linkMerge", member);
    }
    {
         auto member = toYaml(*pickValue, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "pickValue", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::Sink::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["source"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *source);
    }
    {
        auto nodeAsList = convertMapToList(n["linkMerge"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *linkMerge);
    }
    {
        auto nodeAsList = convertMapToList(n["pickValue"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *pickValue);
    }
}
inline auto w3id_org::cwl::cwl::WorkflowStepInput::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("WorkflowStepInput");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::Identified::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::cwl::Sink::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::cwl::LoadContents::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::cwl::Labeled::toYaml(config));
    {
         auto member = toYaml(*default_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "default", member);
    }
    {
         auto member = toYaml(*valueFrom, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "valueFrom", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::WorkflowStepInput::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::Identified::fromYaml(n);
    w3id_org::cwl::cwl::Sink::fromYaml(n);
    w3id_org::cwl::cwl::LoadContents::fromYaml(n);
    w3id_org::cwl::cwl::Labeled::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["default"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *default_);
    }
    {
        auto nodeAsList = convertMapToList(n["valueFrom"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *valueFrom);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::WorkflowStepInput> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::WorkflowStepInput> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::WorkflowStepInput{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.default_)::value_t>::value) try {
            fromYaml(n["default"], *res.default_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.valueFrom)::value_t>::value) try {
            fromYaml(n["valueFrom"], *res.valueFrom);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::WorkflowStepOutput::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("WorkflowStepOutput");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::Identified::toYaml(config));
    return n;
}
inline void w3id_org::cwl::cwl::WorkflowStepOutput::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::Identified::fromYaml(n);
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::WorkflowStepOutput> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::WorkflowStepOutput> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::WorkflowStepOutput{};

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::WorkflowStep::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("WorkflowStep");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::Identified::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::cwl::Labeled::toYaml(config));
    n = mergeYaml(n, w3id_org::cwl::salad::Documented::toYaml(config));
    {
         auto member = toYaml(*in, config);
         member = convertListToMap(member, "id", "source", config);
        addYamlField(n, "in", member);
    }
    {
         auto member = toYaml(*out, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "out", member);
    }
    {
         auto member = toYaml(*requirements, config);
         member = convertListToMap(member, "class", "", config);
        addYamlField(n, "requirements", member);
    }
    {
         auto member = toYaml(*hints, config);
         member = convertListToMap(member, "class", "", config);
        addYamlField(n, "hints", member);
    }
    {
         auto member = toYaml(*run, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "run", member);
    }
    {
         auto member = toYaml(*when, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "when", member);
    }
    {
         auto member = toYaml(*scatter, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "scatter", member);
    }
    {
         auto member = toYaml(*scatterMethod, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "scatterMethod", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::WorkflowStep::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::Identified::fromYaml(n);
    w3id_org::cwl::cwl::Labeled::fromYaml(n);
    w3id_org::cwl::salad::Documented::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["in"], "id", "source");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *in);
    }
    {
        auto nodeAsList = convertMapToList(n["out"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *out);
    }
    {
        auto nodeAsList = convertMapToList(n["requirements"], "class", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *requirements);
    }
    {
        auto nodeAsList = convertMapToList(n["hints"], "class", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *hints);
    }
    {
        auto nodeAsList = convertMapToList(n["run"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *run);
    }
    {
        auto nodeAsList = convertMapToList(n["when"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *when);
    }
    {
        auto nodeAsList = convertMapToList(n["scatter"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *scatter);
    }
    {
        auto nodeAsList = convertMapToList(n["scatterMethod"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *scatterMethod);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::WorkflowStep> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::WorkflowStep> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::WorkflowStep{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.in)::value_t>::value) try {
            fromYaml(n["in"], *res.in);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.out)::value_t>::value) try {
            fromYaml(n["out"], *res.out);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.requirements)::value_t>::value) try {
            fromYaml(n["requirements"], *res.requirements);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.hints)::value_t>::value) try {
            fromYaml(n["hints"], *res.hints);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.run)::value_t>::value) try {
            fromYaml(n["run"], *res.run);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.when)::value_t>::value) try {
            fromYaml(n["when"], *res.when);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.scatter)::value_t>::value) try {
            fromYaml(n["scatter"], *res.scatter);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.scatterMethod)::value_t>::value) try {
            fromYaml(n["scatterMethod"], *res.scatterMethod);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::Workflow::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("Workflow");
    }
    {
         auto member = toYaml(*id, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "id", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*inputs, config);
         member = convertListToMap(member, "id", "type", config);
        addYamlField(n, "inputs", member);
    }
    {
         auto member = toYaml(*outputs, config);
         member = convertListToMap(member, "id", "type", config);
        addYamlField(n, "outputs", member);
    }
    {
         auto member = toYaml(*requirements, config);
         member = convertListToMap(member, "class", "", config);
        addYamlField(n, "requirements", member);
    }
    {
         auto member = toYaml(*hints, config);
         member = convertListToMap(member, "class", "", config);
        addYamlField(n, "hints", member);
    }
    {
         auto member = toYaml(*cwlVersion, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "cwlVersion", member);
    }
    {
         auto member = toYaml(*intent, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "intent", member);
    }
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    {
         auto member = toYaml(*steps, config);
         member = convertListToMap(member, "id", "", config);
        addYamlField(n, "steps", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::Workflow::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["id"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *id);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["inputs"], "id", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputs);
    }
    {
        auto nodeAsList = convertMapToList(n["outputs"], "id", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *outputs);
    }
    {
        auto nodeAsList = convertMapToList(n["requirements"], "class", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *requirements);
    }
    {
        auto nodeAsList = convertMapToList(n["hints"], "class", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *hints);
    }
    {
        auto nodeAsList = convertMapToList(n["cwlVersion"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *cwlVersion);
    }
    {
        auto nodeAsList = convertMapToList(n["intent"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *intent);
    }
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
    {
        auto nodeAsList = convertMapToList(n["steps"], "id", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *steps);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::Workflow> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::Workflow> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::Workflow{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.id)::value_t>::value) try {
            fromYaml(n["id"], *res.id);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.inputs)::value_t>::value) try {
            fromYaml(n["inputs"], *res.inputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.outputs)::value_t>::value) try {
            fromYaml(n["outputs"], *res.outputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.requirements)::value_t>::value) try {
            fromYaml(n["requirements"], *res.requirements);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.hints)::value_t>::value) try {
            fromYaml(n["hints"], *res.hints);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.cwlVersion)::value_t>::value) try {
            fromYaml(n["cwlVersion"], *res.cwlVersion);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.intent)::value_t>::value) try {
            fromYaml(n["intent"], *res.intent);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.steps)::value_t>::value) try {
            fromYaml(n["steps"], *res.steps);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::SubworkflowFeatureRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("SubworkflowFeatureRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::SubworkflowFeatureRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::SubworkflowFeatureRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::SubworkflowFeatureRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::SubworkflowFeatureRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::ScatterFeatureRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ScatterFeatureRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::ScatterFeatureRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::ScatterFeatureRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::ScatterFeatureRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::ScatterFeatureRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::MultipleInputFeatureRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("MultipleInputFeatureRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::MultipleInputFeatureRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::MultipleInputFeatureRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::MultipleInputFeatureRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::MultipleInputFeatureRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::StepInputExpressionRequirement::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("StepInputExpressionRequirement");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::StepInputExpressionRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::StepInputExpressionRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::StepInputExpressionRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::StepInputExpressionRequirement{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::OperationInputParameter::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OperationInputParameter");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::InputParameter::toYaml(config));
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::OperationInputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::InputParameter::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::OperationInputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::OperationInputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::OperationInputParameter{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::OperationOutputParameter::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OperationOutputParameter");
    }
    n = mergeYaml(n, w3id_org::cwl::cwl::OutputParameter::toYaml(config));
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::OperationOutputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    w3id_org::cwl::cwl::OutputParameter::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::OperationOutputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::OperationOutputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::OperationOutputParameter{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
inline auto w3id_org::cwl::cwl::Operation::toYaml([[maybe_unused]] ::w3id_org::cwl::store_config const& config) const -> YAML::Node {
    using ::w3id_org::cwl::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("Operation");
    }
    {
         auto member = toYaml(*id, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "id", member);
    }
    {
         auto member = toYaml(*label, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "label", member);
    }
    {
         auto member = toYaml(*doc, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "doc", member);
    }
    {
         auto member = toYaml(*inputs, config);
         member = convertListToMap(member, "id", "type", config);
        addYamlField(n, "inputs", member);
    }
    {
         auto member = toYaml(*outputs, config);
         member = convertListToMap(member, "id", "type", config);
        addYamlField(n, "outputs", member);
    }
    {
         auto member = toYaml(*requirements, config);
         member = convertListToMap(member, "class", "", config);
        addYamlField(n, "requirements", member);
    }
    {
         auto member = toYaml(*hints, config);
         member = convertListToMap(member, "class", "", config);
        addYamlField(n, "hints", member);
    }
    {
         auto member = toYaml(*cwlVersion, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "cwlVersion", member);
    }
    {
         auto member = toYaml(*intent, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "intent", member);
    }
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    return n;
}
inline void w3id_org::cwl::cwl::Operation::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::w3id_org::cwl::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["id"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *id);
    }
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
    {
        auto nodeAsList = convertMapToList(n["inputs"], "id", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputs);
    }
    {
        auto nodeAsList = convertMapToList(n["outputs"], "id", "type");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *outputs);
    }
    {
        auto nodeAsList = convertMapToList(n["requirements"], "class", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *requirements);
    }
    {
        auto nodeAsList = convertMapToList(n["hints"], "class", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *hints);
    }
    {
        auto nodeAsList = convertMapToList(n["cwlVersion"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *cwlVersion);
    }
    {
        auto nodeAsList = convertMapToList(n["intent"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *intent);
    }
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
}
namespace w3id_org::cwl {
template <>
struct DetectAndExtractFromYaml<::w3id_org::cwl::cwl::Operation> {
    auto operator()(YAML::Node const& n) const -> std::optional<::w3id_org::cwl::cwl::Operation> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = ::w3id_org::cwl::cwl::Operation{};

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.id)::value_t>::value) try {
            fromYaml(n["id"], *res.id);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.inputs)::value_t>::value) try {
            fromYaml(n["inputs"], *res.inputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.outputs)::value_t>::value) try {
            fromYaml(n["outputs"], *res.outputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.requirements)::value_t>::value) try {
            fromYaml(n["requirements"], *res.requirements);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.hints)::value_t>::value) try {
            fromYaml(n["hints"], *res.hints);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.cwlVersion)::value_t>::value) try {
            fromYaml(n["cwlVersion"], *res.cwlVersion);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.intent)::value_t>::value) try {
            fromYaml(n["intent"], *res.intent);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (::w3id_org::cwl::IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
}
namespace w3id_org::cwl {

template <typename T>
auto toYaml(std::vector<T> const& v, [[maybe_unused]] store_config const& config) -> YAML::Node {
    auto n = YAML::Node(YAML::NodeType::Sequence);
    for (auto const& e : v) {
        n.push_back(toYaml(e, config));
    }
    return n;
}

template <typename T>
auto toYaml(std::map<std::string, T> const& v, [[maybe_unused]] store_config const& config) -> YAML::Node {
    auto n = YAML::Node(YAML::NodeType::Map);
    for (auto const& [key, value] : v) {
        n[key] = toYaml(value, config);
    }
    return n;
}

template <typename T>
auto toYaml(T const& t, [[maybe_unused]] store_config const& config) -> YAML::Node {
    if constexpr (std::is_enum_v<T>) {
        return toYaml(t, config);
    } else {
        return t.toYaml(config);
    }
}

template <typename ...Args>
auto toYaml(std::variant<Args...> const& t, store_config const& config) -> YAML::Node {
    return std::visit([config](auto const& e) {
        return toYaml(e, config);
    }, t);
}

template <typename T>
void fromYaml(YAML::Node const& n, std::vector<T>& v){
    if (!n.IsSequence()) return;
    for (auto e : n) {
        v.emplace_back();
        fromYaml(e, v.back());
    }
}

template <typename T>
void fromYaml(YAML::Node const& n, std::map<std::string, T>& v){
    if (!n.IsMap()) return;
    for (auto e : n) {
        auto key = e.first.as<std::string>();
        fromYaml(e.second, v[key]);
    }
}

template <typename T>
void fromYaml(YAML::Node const& n, T& t){
    if constexpr (std::is_enum_v<T>) {
        fromYaml(n, t);
    } else {
        t.fromYaml(n);
    }
}

template <typename SomeVariant, typename Head, typename ...Args>
bool detectAndExtractFromYaml(YAML::Node const& n, SomeVariant& v, Head* = nullptr) {
    auto r = DetectAndExtractFromYaml<Head>{}(n);
    if (r) {
        v = *r;
        return true;
    }
    if constexpr (sizeof...(Args) > 0) {
        return detectAndExtractFromYaml<SomeVariant, Args...>(n, v);
    }
    return false;
}

template <typename SomeVariant, typename Head, typename Tail>
bool detectAndExtractFromYaml(YAML::Node const& n, std::variant<std::monostate, Tail>& v, Head* = nullptr) {
    auto r = DetectAndExtractFromYaml<Head>{}(n);
    if (r) {
        v = *r;
        return true;
    }
    auto t = Tail{};
    fromYaml(n, t);
    v = t;
    return true;
}

template <typename ...Args>
void fromYaml(YAML::Node const& n, std::variant<Args...>& v){
    bool found = detectAndExtractFromYaml<std::variant<Args...>, Args...>(n, v);
    if (!found) throw std::runtime_error{"didn't find any overload"};
}
using DocumentRootType = std::variant<w3id_org::cwl::cwl::CommandLineTool, w3id_org::cwl::cwl::ExpressionTool, w3id_org::cwl::cwl::Workflow, w3id_org::cwl::cwl::Operation>;
auto load_document_from_yaml(YAML::Node n) -> DocumentRootType {
    DocumentRootType root;
    fromYaml(n, root);
    return root;
}
auto load_document_from_string(std::string document) -> DocumentRootType {
    return load_document_from_yaml(YAML::Load(document));
}
auto load_document(std::filesystem::path path) -> DocumentRootType {
    return load_document_from_yaml(YAML::LoadFile(path.string()));
}
void store_document(DocumentRootType const& root, std::ostream& ostream, store_config config={}) {
    auto y = toYaml(root, config);

    YAML::Emitter out;
    out << y;
    ostream << out.c_str() << std::endl;
}
void store_document(DocumentRootType const& root, std::filesystem::path const& path, store_config config={}) {
    auto ofs = std::ofstream{path};
    store_document(root, ofs, config);
}
auto store_document_as_string(DocumentRootType const& root, store_config config={}) -> std::string {
    auto ss = std::stringstream{};
    store_document(root, ss, config);
    return ss.str();
}

}