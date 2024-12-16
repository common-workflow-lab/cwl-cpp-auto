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

namespace cpp_gen {

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

inline auto convertListToMap(YAML::Node list, std::string const& mapSubject, std::string const& mapPredicate, store_config const& config) {
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
inline auto convertMapToList(YAML::Node map, std::string const& mapSubject, std::string const& mapPredicate) {
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

namespace https___w3id_org_cwl_salad { struct Documented; }
namespace https___w3id_org_cwl_salad { struct RecordField; }
namespace https___w3id_org_cwl_salad { struct RecordSchema; }
namespace https___w3id_org_cwl_salad { struct EnumSchema; }
namespace https___w3id_org_cwl_salad { struct ArraySchema; }
namespace https___w3id_org_cwl_cwl { struct File; }
namespace https___w3id_org_cwl_cwl { struct Directory; }
namespace https___w3id_org_cwl_cwl { struct Labeled; }
namespace https___w3id_org_cwl_cwl { struct Identified; }
namespace https___w3id_org_cwl_cwl { struct LoadContents; }
namespace https___w3id_org_cwl_cwl { struct FieldBase; }
namespace https___w3id_org_cwl_cwl { struct InputFormat; }
namespace https___w3id_org_cwl_cwl { struct OutputFormat; }
namespace https___w3id_org_cwl_cwl { struct Parameter; }
namespace https___w3id_org_cwl_cwl { struct InputBinding; }
namespace https___w3id_org_cwl_cwl { struct IOSchema; }
namespace https___w3id_org_cwl_cwl { struct InputSchema; }
namespace https___w3id_org_cwl_cwl { struct OutputSchema; }
namespace https___w3id_org_cwl_cwl { struct InputRecordField; }
namespace https___w3id_org_cwl_cwl { struct InputRecordSchema; }
namespace https___w3id_org_cwl_cwl { struct InputEnumSchema; }
namespace https___w3id_org_cwl_cwl { struct InputArraySchema; }
namespace https___w3id_org_cwl_cwl { struct OutputRecordField; }
namespace https___w3id_org_cwl_cwl { struct OutputRecordSchema; }
namespace https___w3id_org_cwl_cwl { struct OutputEnumSchema; }
namespace https___w3id_org_cwl_cwl { struct OutputArraySchema; }
namespace https___w3id_org_cwl_cwl { struct InputParameter; }
namespace https___w3id_org_cwl_cwl { struct OutputParameter; }
namespace https___w3id_org_cwl_cwl { struct ProcessRequirement; }
namespace https___w3id_org_cwl_cwl { struct Process; }
namespace https___w3id_org_cwl_cwl { struct InlineJavascriptRequirement; }
namespace https___w3id_org_cwl_cwl { struct CommandInputSchema; }
namespace https___w3id_org_cwl_cwl { struct SchemaDefRequirement; }
namespace https___w3id_org_cwl_cwl { struct SecondaryFileSchema; }
namespace https___w3id_org_cwl_cwl { struct LoadListingRequirement; }
namespace https___w3id_org_cwl_cwl { struct EnvironmentDef; }
namespace https___w3id_org_cwl_cwl { struct CommandLineBinding; }
namespace https___w3id_org_cwl_cwl { struct CommandOutputBinding; }
namespace https___w3id_org_cwl_cwl { struct CommandLineBindable; }
namespace https___w3id_org_cwl_cwl { struct CommandInputRecordField; }
namespace https___w3id_org_cwl_cwl { struct CommandInputRecordSchema; }
namespace https___w3id_org_cwl_cwl { struct CommandInputEnumSchema; }
namespace https___w3id_org_cwl_cwl { struct CommandInputArraySchema; }
namespace https___w3id_org_cwl_cwl { struct CommandOutputRecordField; }
namespace https___w3id_org_cwl_cwl { struct CommandOutputRecordSchema; }
namespace https___w3id_org_cwl_cwl { struct CommandOutputEnumSchema; }
namespace https___w3id_org_cwl_cwl { struct CommandOutputArraySchema; }
namespace https___w3id_org_cwl_cwl { struct CommandInputParameter; }
namespace https___w3id_org_cwl_cwl { struct CommandOutputParameter; }
namespace https___w3id_org_cwl_cwl { struct CommandLineTool; }
namespace https___w3id_org_cwl_cwl { struct DockerRequirement; }
namespace https___w3id_org_cwl_cwl { struct SoftwareRequirement; }
namespace https___w3id_org_cwl_cwl { struct SoftwarePackage; }
namespace https___w3id_org_cwl_cwl { struct Dirent; }
namespace https___w3id_org_cwl_cwl { struct InitialWorkDirRequirement; }
namespace https___w3id_org_cwl_cwl { struct EnvVarRequirement; }
namespace https___w3id_org_cwl_cwl { struct ShellCommandRequirement; }
namespace https___w3id_org_cwl_cwl { struct ResourceRequirement; }
namespace https___w3id_org_cwl_cwl { struct WorkReuse; }
namespace https___w3id_org_cwl_cwl { struct NetworkAccess; }
namespace https___w3id_org_cwl_cwl { struct InplaceUpdateRequirement; }
namespace https___w3id_org_cwl_cwl { struct ToolTimeLimit; }
namespace https___w3id_org_cwl_cwl { struct ExpressionToolOutputParameter; }
namespace https___w3id_org_cwl_cwl { struct WorkflowInputParameter; }
namespace https___w3id_org_cwl_cwl { struct ExpressionTool; }
namespace https___w3id_org_cwl_cwl { struct WorkflowOutputParameter; }
namespace https___w3id_org_cwl_cwl { struct Sink; }
namespace https___w3id_org_cwl_cwl { struct WorkflowStepInput; }
namespace https___w3id_org_cwl_cwl { struct WorkflowStepOutput; }
namespace https___w3id_org_cwl_cwl { struct WorkflowStep; }
namespace https___w3id_org_cwl_cwl { struct Workflow; }
namespace https___w3id_org_cwl_cwl { struct SubworkflowFeatureRequirement; }
namespace https___w3id_org_cwl_cwl { struct ScatterFeatureRequirement; }
namespace https___w3id_org_cwl_cwl { struct MultipleInputFeatureRequirement; }
namespace https___w3id_org_cwl_cwl { struct StepInputExpressionRequirement; }
namespace https___w3id_org_cwl_cwl { struct OperationInputParameter; }
namespace https___w3id_org_cwl_cwl { struct OperationOutputParameter; }
namespace https___w3id_org_cwl_cwl { struct Operation; }
namespace https___w3id_org_cwl_salad {
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
    using U = std::underlying_type_t<https___w3id_org_cwl_salad::PrimitiveType>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_salad::PrimitiveType& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_salad::PrimitiveType, std::less<>> {
        {"null", https___w3id_org_cwl_salad::PrimitiveType::null},
        {"boolean", https___w3id_org_cwl_salad::PrimitiveType::boolean},
        {"int", https___w3id_org_cwl_salad::PrimitiveType::int_},
        {"long", https___w3id_org_cwl_salad::PrimitiveType::long_},
        {"float", https___w3id_org_cwl_salad::PrimitiveType::float_},
        {"double", https___w3id_org_cwl_salad::PrimitiveType::double_},
        {"string", https___w3id_org_cwl_salad::PrimitiveType::string},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_salad::PrimitiveType v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_salad::PrimitiveType");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_salad::PrimitiveType& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_salad::PrimitiveType> : std::true_type {};

namespace https___w3id_org_cwl_salad {
enum class Any : unsigned int {
    Any
};
inline auto to_string(Any v) {
    static auto m = std::vector<std::string_view> {
        "Any"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_salad::Any>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_salad::Any& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_salad::Any, std::less<>> {
        {"Any", https___w3id_org_cwl_salad::Any::Any},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_salad::Any v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_salad::Any");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_salad::Any& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_salad::Any> : std::true_type {};

namespace https___w3id_org_cwl_salad {
enum class RecordSchema_type_Record_name : unsigned int {
    record
};
inline auto to_string(RecordSchema_type_Record_name v) {
    static auto m = std::vector<std::string_view> {
        "record"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_salad::RecordSchema_type_Record_name>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_salad::RecordSchema_type_Record_name& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_salad::RecordSchema_type_Record_name, std::less<>> {
        {"record", https___w3id_org_cwl_salad::RecordSchema_type_Record_name::record},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_salad::RecordSchema_type_Record_name v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_salad::RecordSchema_type_Record_name");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_salad::RecordSchema_type_Record_name& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_salad::RecordSchema_type_Record_name> : std::true_type {};

namespace https___w3id_org_cwl_salad {
enum class EnumSchema_type_Enum_name : unsigned int {
    enum_
};
inline auto to_string(EnumSchema_type_Enum_name v) {
    static auto m = std::vector<std::string_view> {
        "enum"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_salad::EnumSchema_type_Enum_name>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_salad::EnumSchema_type_Enum_name& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_salad::EnumSchema_type_Enum_name, std::less<>> {
        {"enum", https___w3id_org_cwl_salad::EnumSchema_type_Enum_name::enum_},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_salad::EnumSchema_type_Enum_name v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_salad::EnumSchema_type_Enum_name");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_salad::EnumSchema_type_Enum_name& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_salad::EnumSchema_type_Enum_name> : std::true_type {};

namespace https___w3id_org_cwl_salad {
enum class ArraySchema_type_Array_name : unsigned int {
    array
};
inline auto to_string(ArraySchema_type_Array_name v) {
    static auto m = std::vector<std::string_view> {
        "array"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_salad::ArraySchema_type_Array_name>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_salad::ArraySchema_type_Array_name& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_salad::ArraySchema_type_Array_name, std::less<>> {
        {"array", https___w3id_org_cwl_salad::ArraySchema_type_Array_name::array},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_salad::ArraySchema_type_Array_name v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_salad::ArraySchema_type_Array_name");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_salad::ArraySchema_type_Array_name& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_salad::ArraySchema_type_Array_name> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
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
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::CWLVersion>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::CWLVersion& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::CWLVersion, std::less<>> {
        {"draft-2", https___w3id_org_cwl_cwl::CWLVersion::draft_2},
        {"draft-3.dev1", https___w3id_org_cwl_cwl::CWLVersion::draft_3_dev1},
        {"draft-3.dev2", https___w3id_org_cwl_cwl::CWLVersion::draft_3_dev2},
        {"draft-3.dev3", https___w3id_org_cwl_cwl::CWLVersion::draft_3_dev3},
        {"draft-3.dev4", https___w3id_org_cwl_cwl::CWLVersion::draft_3_dev4},
        {"draft-3.dev5", https___w3id_org_cwl_cwl::CWLVersion::draft_3_dev5},
        {"draft-3", https___w3id_org_cwl_cwl::CWLVersion::draft_3},
        {"draft-4.dev1", https___w3id_org_cwl_cwl::CWLVersion::draft_4_dev1},
        {"draft-4.dev2", https___w3id_org_cwl_cwl::CWLVersion::draft_4_dev2},
        {"draft-4.dev3", https___w3id_org_cwl_cwl::CWLVersion::draft_4_dev3},
        {"v1.0.dev4", https___w3id_org_cwl_cwl::CWLVersion::v1_0_dev4},
        {"v1.0", https___w3id_org_cwl_cwl::CWLVersion::v1_0},
        {"v1.1.0-dev1", https___w3id_org_cwl_cwl::CWLVersion::v1_1_0_dev1},
        {"v1.1", https___w3id_org_cwl_cwl::CWLVersion::v1_1},
        {"v1.2.0-dev1", https___w3id_org_cwl_cwl::CWLVersion::v1_2_0_dev1},
        {"v1.2.0-dev2", https___w3id_org_cwl_cwl::CWLVersion::v1_2_0_dev2},
        {"v1.2.0-dev3", https___w3id_org_cwl_cwl::CWLVersion::v1_2_0_dev3},
        {"v1.2.0-dev4", https___w3id_org_cwl_cwl::CWLVersion::v1_2_0_dev4},
        {"v1.2.0-dev5", https___w3id_org_cwl_cwl::CWLVersion::v1_2_0_dev5},
        {"v1.2", https___w3id_org_cwl_cwl::CWLVersion::v1_2},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::CWLVersion v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::CWLVersion");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::CWLVersion& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::CWLVersion> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
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
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::CWLType>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::CWLType& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::CWLType, std::less<>> {
        {"null", https___w3id_org_cwl_cwl::CWLType::null},
        {"boolean", https___w3id_org_cwl_cwl::CWLType::boolean},
        {"int", https___w3id_org_cwl_cwl::CWLType::int_},
        {"long", https___w3id_org_cwl_cwl::CWLType::long_},
        {"float", https___w3id_org_cwl_cwl::CWLType::float_},
        {"double", https___w3id_org_cwl_cwl::CWLType::double_},
        {"string", https___w3id_org_cwl_cwl::CWLType::string},
        {"File", https___w3id_org_cwl_cwl::CWLType::File},
        {"Directory", https___w3id_org_cwl_cwl::CWLType::Directory},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::CWLType v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::CWLType");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::CWLType& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::CWLType> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class File_class_File_class : unsigned int {
    File
};
inline auto to_string(File_class_File_class v) {
    static auto m = std::vector<std::string_view> {
        "File"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::File_class_File_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::File_class_File_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::File_class_File_class, std::less<>> {
        {"File", https___w3id_org_cwl_cwl::File_class_File_class::File},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::File_class_File_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::File_class_File_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::File_class_File_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::File_class_File_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class Directory_class_Directory_class : unsigned int {
    Directory
};
inline auto to_string(Directory_class_Directory_class v) {
    static auto m = std::vector<std::string_view> {
        "Directory"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::Directory_class_Directory_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::Directory_class_Directory_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::Directory_class_Directory_class, std::less<>> {
        {"Directory", https___w3id_org_cwl_cwl::Directory_class_Directory_class::Directory},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::Directory_class_Directory_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::Directory_class_Directory_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::Directory_class_Directory_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::Directory_class_Directory_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
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
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::LoadListingEnum>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::LoadListingEnum& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::LoadListingEnum, std::less<>> {
        {"no_listing", https___w3id_org_cwl_cwl::LoadListingEnum::no_listing},
        {"shallow_listing", https___w3id_org_cwl_cwl::LoadListingEnum::shallow_listing},
        {"deep_listing", https___w3id_org_cwl_cwl::LoadListingEnum::deep_listing},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::LoadListingEnum v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::LoadListingEnum");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::LoadListingEnum& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::LoadListingEnum> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class Expression : unsigned int {
    ExpressionPlaceholder
};
inline auto to_string(Expression v) {
    static auto m = std::vector<std::string_view> {
        "ExpressionPlaceholder"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::Expression>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::Expression& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::Expression, std::less<>> {
        {"ExpressionPlaceholder", https___w3id_org_cwl_cwl::Expression::ExpressionPlaceholder},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::Expression v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::Expression");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::Expression& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::Expression> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class InlineJavascriptRequirement_class_InlineJavascriptRequirement_class : unsigned int {
    InlineJavascriptRequirement
};
inline auto to_string(InlineJavascriptRequirement_class_InlineJavascriptRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "InlineJavascriptRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class, std::less<>> {
        {"InlineJavascriptRequirement", https___w3id_org_cwl_cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class::InlineJavascriptRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::InlineJavascriptRequirement_class_InlineJavascriptRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class SchemaDefRequirement_class_SchemaDefRequirement_class : unsigned int {
    SchemaDefRequirement
};
inline auto to_string(SchemaDefRequirement_class_SchemaDefRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "SchemaDefRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::SchemaDefRequirement_class_SchemaDefRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::SchemaDefRequirement_class_SchemaDefRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::SchemaDefRequirement_class_SchemaDefRequirement_class, std::less<>> {
        {"SchemaDefRequirement", https___w3id_org_cwl_cwl::SchemaDefRequirement_class_SchemaDefRequirement_class::SchemaDefRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::SchemaDefRequirement_class_SchemaDefRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::SchemaDefRequirement_class_SchemaDefRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::SchemaDefRequirement_class_SchemaDefRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::SchemaDefRequirement_class_SchemaDefRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class LoadListingRequirement_class_LoadListingRequirement_class : unsigned int {
    LoadListingRequirement
};
inline auto to_string(LoadListingRequirement_class_LoadListingRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "LoadListingRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::LoadListingRequirement_class_LoadListingRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::LoadListingRequirement_class_LoadListingRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::LoadListingRequirement_class_LoadListingRequirement_class, std::less<>> {
        {"LoadListingRequirement", https___w3id_org_cwl_cwl::LoadListingRequirement_class_LoadListingRequirement_class::LoadListingRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::LoadListingRequirement_class_LoadListingRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::LoadListingRequirement_class_LoadListingRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::LoadListingRequirement_class_LoadListingRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::LoadListingRequirement_class_LoadListingRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class stdin_ : unsigned int {
    stdin_
};
inline auto to_string(stdin_ v) {
    static auto m = std::vector<std::string_view> {
        "stdin"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::stdin_>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::stdin_& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::stdin_, std::less<>> {
        {"stdin", https___w3id_org_cwl_cwl::stdin_::stdin_},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::stdin_ v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::stdin_");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::stdin_& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::stdin_> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class stdout_ : unsigned int {
    stdout_
};
inline auto to_string(stdout_ v) {
    static auto m = std::vector<std::string_view> {
        "stdout"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::stdout_>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::stdout_& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::stdout_, std::less<>> {
        {"stdout", https___w3id_org_cwl_cwl::stdout_::stdout_},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::stdout_ v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::stdout_");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::stdout_& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::stdout_> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class stderr_ : unsigned int {
    stderr_
};
inline auto to_string(stderr_ v) {
    static auto m = std::vector<std::string_view> {
        "stderr"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::stderr_>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::stderr_& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::stderr_, std::less<>> {
        {"stderr", https___w3id_org_cwl_cwl::stderr_::stderr_},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::stderr_ v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::stderr_");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::stderr_& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::stderr_> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class CommandLineTool_class_CommandLineTool_class : unsigned int {
    CommandLineTool
};
inline auto to_string(CommandLineTool_class_CommandLineTool_class v) {
    static auto m = std::vector<std::string_view> {
        "CommandLineTool"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::CommandLineTool_class_CommandLineTool_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::CommandLineTool_class_CommandLineTool_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::CommandLineTool_class_CommandLineTool_class, std::less<>> {
        {"CommandLineTool", https___w3id_org_cwl_cwl::CommandLineTool_class_CommandLineTool_class::CommandLineTool},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::CommandLineTool_class_CommandLineTool_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::CommandLineTool_class_CommandLineTool_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::CommandLineTool_class_CommandLineTool_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::CommandLineTool_class_CommandLineTool_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class DockerRequirement_class_DockerRequirement_class : unsigned int {
    DockerRequirement
};
inline auto to_string(DockerRequirement_class_DockerRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "DockerRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::DockerRequirement_class_DockerRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::DockerRequirement_class_DockerRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::DockerRequirement_class_DockerRequirement_class, std::less<>> {
        {"DockerRequirement", https___w3id_org_cwl_cwl::DockerRequirement_class_DockerRequirement_class::DockerRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::DockerRequirement_class_DockerRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::DockerRequirement_class_DockerRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::DockerRequirement_class_DockerRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::DockerRequirement_class_DockerRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class SoftwareRequirement_class_SoftwareRequirement_class : unsigned int {
    SoftwareRequirement
};
inline auto to_string(SoftwareRequirement_class_SoftwareRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "SoftwareRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::SoftwareRequirement_class_SoftwareRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::SoftwareRequirement_class_SoftwareRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::SoftwareRequirement_class_SoftwareRequirement_class, std::less<>> {
        {"SoftwareRequirement", https___w3id_org_cwl_cwl::SoftwareRequirement_class_SoftwareRequirement_class::SoftwareRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::SoftwareRequirement_class_SoftwareRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::SoftwareRequirement_class_SoftwareRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::SoftwareRequirement_class_SoftwareRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::SoftwareRequirement_class_SoftwareRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class InitialWorkDirRequirement_class_InitialWorkDirRequirement_class : unsigned int {
    InitialWorkDirRequirement
};
inline auto to_string(InitialWorkDirRequirement_class_InitialWorkDirRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "InitialWorkDirRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class, std::less<>> {
        {"InitialWorkDirRequirement", https___w3id_org_cwl_cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class::InitialWorkDirRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::InitialWorkDirRequirement_class_InitialWorkDirRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class EnvVarRequirement_class_EnvVarRequirement_class : unsigned int {
    EnvVarRequirement
};
inline auto to_string(EnvVarRequirement_class_EnvVarRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "EnvVarRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::EnvVarRequirement_class_EnvVarRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::EnvVarRequirement_class_EnvVarRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::EnvVarRequirement_class_EnvVarRequirement_class, std::less<>> {
        {"EnvVarRequirement", https___w3id_org_cwl_cwl::EnvVarRequirement_class_EnvVarRequirement_class::EnvVarRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::EnvVarRequirement_class_EnvVarRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::EnvVarRequirement_class_EnvVarRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::EnvVarRequirement_class_EnvVarRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::EnvVarRequirement_class_EnvVarRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class ShellCommandRequirement_class_ShellCommandRequirement_class : unsigned int {
    ShellCommandRequirement
};
inline auto to_string(ShellCommandRequirement_class_ShellCommandRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "ShellCommandRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::ShellCommandRequirement_class_ShellCommandRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::ShellCommandRequirement_class_ShellCommandRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::ShellCommandRequirement_class_ShellCommandRequirement_class, std::less<>> {
        {"ShellCommandRequirement", https___w3id_org_cwl_cwl::ShellCommandRequirement_class_ShellCommandRequirement_class::ShellCommandRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::ShellCommandRequirement_class_ShellCommandRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::ShellCommandRequirement_class_ShellCommandRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::ShellCommandRequirement_class_ShellCommandRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::ShellCommandRequirement_class_ShellCommandRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class ResourceRequirement_class_ResourceRequirement_class : unsigned int {
    ResourceRequirement
};
inline auto to_string(ResourceRequirement_class_ResourceRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "ResourceRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::ResourceRequirement_class_ResourceRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::ResourceRequirement_class_ResourceRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::ResourceRequirement_class_ResourceRequirement_class, std::less<>> {
        {"ResourceRequirement", https___w3id_org_cwl_cwl::ResourceRequirement_class_ResourceRequirement_class::ResourceRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::ResourceRequirement_class_ResourceRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::ResourceRequirement_class_ResourceRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::ResourceRequirement_class_ResourceRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::ResourceRequirement_class_ResourceRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class WorkReuse_class_WorkReuse_class : unsigned int {
    WorkReuse
};
inline auto to_string(WorkReuse_class_WorkReuse_class v) {
    static auto m = std::vector<std::string_view> {
        "WorkReuse"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::WorkReuse_class_WorkReuse_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::WorkReuse_class_WorkReuse_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::WorkReuse_class_WorkReuse_class, std::less<>> {
        {"WorkReuse", https___w3id_org_cwl_cwl::WorkReuse_class_WorkReuse_class::WorkReuse},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::WorkReuse_class_WorkReuse_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::WorkReuse_class_WorkReuse_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::WorkReuse_class_WorkReuse_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::WorkReuse_class_WorkReuse_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class NetworkAccess_class_NetworkAccess_class : unsigned int {
    NetworkAccess
};
inline auto to_string(NetworkAccess_class_NetworkAccess_class v) {
    static auto m = std::vector<std::string_view> {
        "NetworkAccess"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::NetworkAccess_class_NetworkAccess_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::NetworkAccess_class_NetworkAccess_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::NetworkAccess_class_NetworkAccess_class, std::less<>> {
        {"NetworkAccess", https___w3id_org_cwl_cwl::NetworkAccess_class_NetworkAccess_class::NetworkAccess},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::NetworkAccess_class_NetworkAccess_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::NetworkAccess_class_NetworkAccess_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::NetworkAccess_class_NetworkAccess_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::NetworkAccess_class_NetworkAccess_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class InplaceUpdateRequirement_class_InplaceUpdateRequirement_class : unsigned int {
    InplaceUpdateRequirement
};
inline auto to_string(InplaceUpdateRequirement_class_InplaceUpdateRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "InplaceUpdateRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class, std::less<>> {
        {"InplaceUpdateRequirement", https___w3id_org_cwl_cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class::InplaceUpdateRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::InplaceUpdateRequirement_class_InplaceUpdateRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class ToolTimeLimit_class_ToolTimeLimit_class : unsigned int {
    ToolTimeLimit
};
inline auto to_string(ToolTimeLimit_class_ToolTimeLimit_class v) {
    static auto m = std::vector<std::string_view> {
        "ToolTimeLimit"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::ToolTimeLimit_class_ToolTimeLimit_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::ToolTimeLimit_class_ToolTimeLimit_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::ToolTimeLimit_class_ToolTimeLimit_class, std::less<>> {
        {"ToolTimeLimit", https___w3id_org_cwl_cwl::ToolTimeLimit_class_ToolTimeLimit_class::ToolTimeLimit},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::ToolTimeLimit_class_ToolTimeLimit_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::ToolTimeLimit_class_ToolTimeLimit_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::ToolTimeLimit_class_ToolTimeLimit_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::ToolTimeLimit_class_ToolTimeLimit_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class ExpressionTool_class_ExpressionTool_class : unsigned int {
    ExpressionTool
};
inline auto to_string(ExpressionTool_class_ExpressionTool_class v) {
    static auto m = std::vector<std::string_view> {
        "ExpressionTool"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::ExpressionTool_class_ExpressionTool_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::ExpressionTool_class_ExpressionTool_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::ExpressionTool_class_ExpressionTool_class, std::less<>> {
        {"ExpressionTool", https___w3id_org_cwl_cwl::ExpressionTool_class_ExpressionTool_class::ExpressionTool},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::ExpressionTool_class_ExpressionTool_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::ExpressionTool_class_ExpressionTool_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::ExpressionTool_class_ExpressionTool_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::ExpressionTool_class_ExpressionTool_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class LinkMergeMethod : unsigned int {
    merge_nested,
    merge_flattened
};
inline auto to_string(LinkMergeMethod v) {
    static auto m = std::vector<std::string_view> {
        "merge_nested",
        "merge_flattened"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::LinkMergeMethod>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::LinkMergeMethod& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::LinkMergeMethod, std::less<>> {
        {"merge_nested", https___w3id_org_cwl_cwl::LinkMergeMethod::merge_nested},
        {"merge_flattened", https___w3id_org_cwl_cwl::LinkMergeMethod::merge_flattened},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::LinkMergeMethod v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::LinkMergeMethod");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::LinkMergeMethod& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::LinkMergeMethod> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
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
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::PickValueMethod>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::PickValueMethod& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::PickValueMethod, std::less<>> {
        {"first_non_null", https___w3id_org_cwl_cwl::PickValueMethod::first_non_null},
        {"the_only_non_null", https___w3id_org_cwl_cwl::PickValueMethod::the_only_non_null},
        {"all_non_null", https___w3id_org_cwl_cwl::PickValueMethod::all_non_null},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::PickValueMethod v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::PickValueMethod");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::PickValueMethod& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::PickValueMethod> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
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
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::ScatterMethod>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::ScatterMethod& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::ScatterMethod, std::less<>> {
        {"dotproduct", https___w3id_org_cwl_cwl::ScatterMethod::dotproduct},
        {"nested_crossproduct", https___w3id_org_cwl_cwl::ScatterMethod::nested_crossproduct},
        {"flat_crossproduct", https___w3id_org_cwl_cwl::ScatterMethod::flat_crossproduct},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::ScatterMethod v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::ScatterMethod");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::ScatterMethod& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::ScatterMethod> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class Workflow_class_Workflow_class : unsigned int {
    Workflow
};
inline auto to_string(Workflow_class_Workflow_class v) {
    static auto m = std::vector<std::string_view> {
        "Workflow"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::Workflow_class_Workflow_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::Workflow_class_Workflow_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::Workflow_class_Workflow_class, std::less<>> {
        {"Workflow", https___w3id_org_cwl_cwl::Workflow_class_Workflow_class::Workflow},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::Workflow_class_Workflow_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::Workflow_class_Workflow_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::Workflow_class_Workflow_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::Workflow_class_Workflow_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class : unsigned int {
    SubworkflowFeatureRequirement
};
inline auto to_string(SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "SubworkflowFeatureRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class, std::less<>> {
        {"SubworkflowFeatureRequirement", https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class::SubworkflowFeatureRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class ScatterFeatureRequirement_class_ScatterFeatureRequirement_class : unsigned int {
    ScatterFeatureRequirement
};
inline auto to_string(ScatterFeatureRequirement_class_ScatterFeatureRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "ScatterFeatureRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class, std::less<>> {
        {"ScatterFeatureRequirement", https___w3id_org_cwl_cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class::ScatterFeatureRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::ScatterFeatureRequirement_class_ScatterFeatureRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class : unsigned int {
    MultipleInputFeatureRequirement
};
inline auto to_string(MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "MultipleInputFeatureRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class, std::less<>> {
        {"MultipleInputFeatureRequirement", https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class::MultipleInputFeatureRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class StepInputExpressionRequirement_class_StepInputExpressionRequirement_class : unsigned int {
    StepInputExpressionRequirement
};
inline auto to_string(StepInputExpressionRequirement_class_StepInputExpressionRequirement_class v) {
    static auto m = std::vector<std::string_view> {
        "StepInputExpressionRequirement"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class, std::less<>> {
        {"StepInputExpressionRequirement", https___w3id_org_cwl_cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class::StepInputExpressionRequirement},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::StepInputExpressionRequirement_class_StepInputExpressionRequirement_class> : std::true_type {};

namespace https___w3id_org_cwl_cwl {
enum class Operation_class_Operation_class : unsigned int {
    Operation
};
inline auto to_string(Operation_class_Operation_class v) {
    static auto m = std::vector<std::string_view> {
        "Operation"
    };
    using U = std::underlying_type_t<https___w3id_org_cwl_cwl::Operation_class_Operation_class>;
    return m.at(static_cast<U>(v));
}
}
inline void to_enum(std::string_view v, https___w3id_org_cwl_cwl::Operation_class_Operation_class& out) {
    static auto m = std::map<std::string, https___w3id_org_cwl_cwl::Operation_class_Operation_class, std::less<>> {
        {"Operation", https___w3id_org_cwl_cwl::Operation_class_Operation_class::Operation},
    };
    auto iter = m.find(v);
    if (iter == m.end()) throw bool{};
    out = iter->second;
}
inline auto toYaml(https___w3id_org_cwl_cwl::Operation_class_Operation_class v, [[maybe_unused]] store_config const& config) {
    auto n = YAML::Node{std::string{to_string(v)}};
    if (config.generateTags) n.SetTag("https___w3id_org_cwl_cwl::Operation_class_Operation_class");
    return n;
}
inline void fromYaml(YAML::Node n, https___w3id_org_cwl_cwl::Operation_class_Operation_class& out) {
    to_enum(n.as<std::string>(), out);
}
template <> struct IsConstant<https___w3id_org_cwl_cwl::Operation_class_Operation_class> : std::true_type {};

namespace https___w3id_org_cwl_salad {
struct Documented {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    virtual ~Documented() = 0;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_salad {
struct RecordField
    : https___w3id_org_cwl_salad::Documented {
    heap_object<std::string> name;
    heap_object<std::variant<std::variant<bool, int32_t, int64_t, float, double, std::string>, RecordSchema, EnumSchema, ArraySchema, std::string, std::vector<std::variant<std::variant<bool, int32_t, int64_t, float, double, std::string>, RecordSchema, EnumSchema, ArraySchema, std::string>>>> type;
    ~RecordField() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_salad {
struct RecordSchema {
    heap_object<std::variant<std::monostate, std::vector<RecordField>>> fields;
    heap_object<RecordSchema_type_Record_name> type;
    virtual ~RecordSchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_salad {
struct EnumSchema {
    heap_object<std::variant<std::monostate, std::string>> name;
    heap_object<std::vector<std::string>> symbols;
    heap_object<EnumSchema_type_Enum_name> type;
    virtual ~EnumSchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_salad {
struct ArraySchema {
    heap_object<std::variant<std::variant<bool, int32_t, int64_t, float, double, std::string>, RecordSchema, EnumSchema, ArraySchema, std::string, std::vector<std::variant<std::variant<bool, int32_t, int64_t, float, double, std::string>, RecordSchema, EnumSchema, ArraySchema, std::string>>>> items;
    heap_object<ArraySchema_type_Array_name> type;
    virtual ~ArraySchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
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
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct Directory {
    heap_object<Directory_class_Directory_class> class_;
    heap_object<std::variant<std::monostate, std::string>> location;
    heap_object<std::variant<std::monostate, std::string>> path;
    heap_object<std::variant<std::monostate, std::string>> basename;
    heap_object<std::variant<std::monostate, std::vector<std::variant<File, Directory>>>> listing;
    virtual ~Directory() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct Labeled {
    heap_object<std::variant<std::monostate, std::string>> label;
    virtual ~Labeled() = 0;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct Identified {
    heap_object<std::variant<std::monostate, std::string>> id;
    virtual ~Identified() = 0;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct LoadContents {
    heap_object<std::variant<std::monostate, bool>> loadContents;
    heap_object<std::variant<std::monostate, LoadListingEnum>> loadListing;
    virtual ~LoadContents() = 0;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct FieldBase
    : https___w3id_org_cwl_cwl::Labeled {
    heap_object<std::variant<std::monostate, SecondaryFileSchema, std::vector<SecondaryFileSchema>>> secondaryFiles;
    heap_object<std::variant<std::monostate, bool>> streamable;
    virtual ~FieldBase() = 0;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct InputFormat {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>, cwl_expression_string>> format;
    virtual ~InputFormat() = 0;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct OutputFormat {
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> format;
    virtual ~OutputFormat() = 0;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct Parameter
    : https___w3id_org_cwl_cwl::FieldBase
    , https___w3id_org_cwl_salad::Documented
    , https___w3id_org_cwl_cwl::Identified {
    virtual ~Parameter() = 0;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct InputBinding {
    heap_object<std::variant<std::monostate, bool>> loadContents;
    virtual ~InputBinding() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct IOSchema
    : https___w3id_org_cwl_cwl::Labeled
    , https___w3id_org_cwl_salad::Documented {
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~IOSchema() = 0;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct InputSchema
    : https___w3id_org_cwl_cwl::IOSchema {
    virtual ~InputSchema() = 0;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct OutputSchema
    : https___w3id_org_cwl_cwl::IOSchema {
    virtual ~OutputSchema() = 0;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
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
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct InputRecordSchema {
    heap_object<std::variant<std::monostate, std::vector<InputRecordField>>> fields;
    heap_object<https___w3id_org_cwl_salad::RecordSchema_type_Record_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~InputRecordSchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct InputEnumSchema
    : https___w3id_org_cwl_salad::EnumSchema
    , https___w3id_org_cwl_cwl::InputSchema {
    ~InputEnumSchema() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct InputArraySchema {
    heap_object<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string, std::vector<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string>>>> items;
    heap_object<https___w3id_org_cwl_salad::ArraySchema_type_Array_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~InputArraySchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct OutputRecordField {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::string> name;
    heap_object<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string, std::vector<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string>>>> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, SecondaryFileSchema, std::vector<SecondaryFileSchema>>> secondaryFiles;
    heap_object<std::variant<std::monostate, bool>> streamable;
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> format;
    virtual ~OutputRecordField() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct OutputRecordSchema {
    heap_object<std::variant<std::monostate, std::vector<OutputRecordField>>> fields;
    heap_object<https___w3id_org_cwl_salad::RecordSchema_type_Record_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~OutputRecordSchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct OutputEnumSchema
    : https___w3id_org_cwl_salad::EnumSchema
    , https___w3id_org_cwl_cwl::OutputSchema {
    ~OutputEnumSchema() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct OutputArraySchema {
    heap_object<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string, std::vector<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string>>>> items;
    heap_object<https___w3id_org_cwl_salad::ArraySchema_type_Array_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~OutputArraySchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct InputParameter
    : https___w3id_org_cwl_cwl::Parameter
    , https___w3id_org_cwl_cwl::InputFormat
    , https___w3id_org_cwl_cwl::LoadContents {
    heap_object<std::variant<std::monostate, File, Directory, std::any>> default_;
    virtual ~InputParameter() = 0;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct OutputParameter
    : https___w3id_org_cwl_cwl::Parameter
    , https___w3id_org_cwl_cwl::OutputFormat {
    virtual ~OutputParameter() = 0;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct ProcessRequirement {
    virtual ~ProcessRequirement() = 0;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct Process
    : https___w3id_org_cwl_cwl::Identified
    , https___w3id_org_cwl_cwl::Labeled
    , https___w3id_org_cwl_salad::Documented {
    heap_object<std::vector<std::variant<CommandInputParameter, WorkflowInputParameter, OperationInputParameter>>> inputs;
    heap_object<std::vector<std::variant<CommandOutputParameter, ExpressionToolOutputParameter, WorkflowOutputParameter, OperationOutputParameter>>> outputs;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement>>>> requirements;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement, std::any>>>> hints;
    heap_object<std::variant<std::monostate, CWLVersion>> cwlVersion;
    heap_object<std::variant<std::monostate, std::vector<std::string>>> intent;
    virtual ~Process() = 0;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct InlineJavascriptRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<InlineJavascriptRequirement_class_InlineJavascriptRequirement_class> class_;
    heap_object<std::variant<std::monostate, std::vector<std::string>>> expressionLib;
    ~InlineJavascriptRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct CommandInputSchema {
    virtual ~CommandInputSchema() = 0;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct SchemaDefRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<SchemaDefRequirement_class_SchemaDefRequirement_class> class_;
    heap_object<std::vector<std::variant<CommandInputRecordSchema, CommandInputEnumSchema, CommandInputArraySchema>>> types;
    ~SchemaDefRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct SecondaryFileSchema {
    heap_object<std::variant<std::string, cwl_expression_string>> pattern;
    heap_object<std::variant<std::monostate, bool, cwl_expression_string>> required;
    virtual ~SecondaryFileSchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct LoadListingRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<LoadListingRequirement_class_LoadListingRequirement_class> class_;
    heap_object<std::variant<std::monostate, LoadListingEnum>> loadListing;
    ~LoadListingRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct EnvironmentDef {
    heap_object<std::string> envName;
    heap_object<std::variant<std::string, cwl_expression_string>> envValue;
    virtual ~EnvironmentDef() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct CommandLineBinding
    : https___w3id_org_cwl_cwl::InputBinding {
    heap_object<std::variant<std::monostate, int32_t, cwl_expression_string>> position;
    heap_object<std::variant<std::monostate, std::string>> prefix;
    heap_object<std::variant<std::monostate, bool>> separate;
    heap_object<std::variant<std::monostate, std::string>> itemSeparator;
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> valueFrom;
    heap_object<std::variant<std::monostate, bool>> shellQuote;
    ~CommandLineBinding() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct CommandOutputBinding
    : https___w3id_org_cwl_cwl::LoadContents {
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string, std::vector<std::string>>> glob;
    heap_object<std::variant<std::monostate, cwl_expression_string>> outputEval;
    ~CommandOutputBinding() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct CommandLineBindable {
    heap_object<std::variant<std::monostate, CommandLineBinding>> inputBinding;
    virtual ~CommandLineBindable() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
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
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct CommandInputRecordSchema {
    heap_object<std::variant<std::monostate, std::vector<CommandInputRecordField>>> fields;
    heap_object<https___w3id_org_cwl_salad::RecordSchema_type_Record_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    heap_object<std::variant<std::monostate, CommandLineBinding>> inputBinding;
    virtual ~CommandInputRecordSchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct CommandInputEnumSchema {
    heap_object<std::variant<std::monostate, std::string>> name;
    heap_object<std::vector<std::string>> symbols;
    heap_object<https___w3id_org_cwl_salad::EnumSchema_type_Enum_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, CommandLineBinding>> inputBinding;
    virtual ~CommandInputEnumSchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct CommandInputArraySchema {
    heap_object<std::variant<CWLType, CommandInputRecordSchema, CommandInputEnumSchema, CommandInputArraySchema, std::string, std::vector<std::variant<CWLType, CommandInputRecordSchema, CommandInputEnumSchema, CommandInputArraySchema, std::string>>>> items;
    heap_object<https___w3id_org_cwl_salad::ArraySchema_type_Array_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    heap_object<std::variant<std::monostate, CommandLineBinding>> inputBinding;
    virtual ~CommandInputArraySchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
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
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct CommandOutputRecordSchema {
    heap_object<std::variant<std::monostate, std::vector<CommandOutputRecordField>>> fields;
    heap_object<https___w3id_org_cwl_salad::RecordSchema_type_Record_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~CommandOutputRecordSchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct CommandOutputEnumSchema {
    heap_object<std::variant<std::monostate, std::string>> name;
    heap_object<std::vector<std::string>> symbols;
    heap_object<https___w3id_org_cwl_salad::EnumSchema_type_Enum_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    virtual ~CommandOutputEnumSchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct CommandOutputArraySchema {
    heap_object<std::variant<CWLType, CommandOutputRecordSchema, CommandOutputEnumSchema, CommandOutputArraySchema, std::string, std::vector<std::variant<CWLType, CommandOutputRecordSchema, CommandOutputEnumSchema, CommandOutputArraySchema, std::string>>>> items;
    heap_object<https___w3id_org_cwl_salad::ArraySchema_type_Array_name> type;
    heap_object<std::variant<std::monostate, std::string>> label;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> doc;
    heap_object<std::variant<std::monostate, std::string>> name;
    virtual ~CommandOutputArraySchema() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct CommandInputParameter
    : https___w3id_org_cwl_cwl::InputParameter {
    heap_object<std::variant<CWLType, stdin_, CommandInputRecordSchema, CommandInputEnumSchema, CommandInputArraySchema, std::string, std::vector<std::variant<CWLType, CommandInputRecordSchema, CommandInputEnumSchema, CommandInputArraySchema, std::string>>>> type;
    heap_object<std::variant<std::monostate, CommandLineBinding>> inputBinding;
    ~CommandInputParameter() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct CommandOutputParameter
    : https___w3id_org_cwl_cwl::OutputParameter {
    heap_object<std::variant<CWLType, stdout_, stderr_, CommandOutputRecordSchema, CommandOutputEnumSchema, CommandOutputArraySchema, std::string, std::vector<std::variant<CWLType, CommandOutputRecordSchema, CommandOutputEnumSchema, CommandOutputArraySchema, std::string>>>> type;
    heap_object<std::variant<std::monostate, CommandOutputBinding>> outputBinding;
    ~CommandOutputParameter() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
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
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct DockerRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<DockerRequirement_class_DockerRequirement_class> class_;
    heap_object<std::variant<std::monostate, std::string>> dockerPull;
    heap_object<std::variant<std::monostate, std::string>> dockerLoad;
    heap_object<std::variant<std::monostate, std::string>> dockerFile;
    heap_object<std::variant<std::monostate, std::string>> dockerImport;
    heap_object<std::variant<std::monostate, std::string>> dockerImageId;
    heap_object<std::variant<std::monostate, std::string>> dockerOutputDirectory;
    ~DockerRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct SoftwareRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<SoftwareRequirement_class_SoftwareRequirement_class> class_;
    heap_object<std::vector<SoftwarePackage>> packages;
    ~SoftwareRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct SoftwarePackage {
    heap_object<std::string> package;
    heap_object<std::variant<std::monostate, std::vector<std::string>>> version;
    heap_object<std::variant<std::monostate, std::vector<std::string>>> specs;
    virtual ~SoftwarePackage() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct Dirent {
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> entryname;
    heap_object<std::variant<std::string, cwl_expression_string>> entry;
    heap_object<std::variant<std::monostate, bool>> writable;
    virtual ~Dirent() = default;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct InitialWorkDirRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<InitialWorkDirRequirement_class_InitialWorkDirRequirement_class> class_;
    heap_object<std::variant<cwl_expression_string, std::vector<std::variant<std::monostate, Dirent, cwl_expression_string, File, Directory, std::vector<std::variant<File, Directory>>>>>> listing;
    ~InitialWorkDirRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct EnvVarRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<EnvVarRequirement_class_EnvVarRequirement_class> class_;
    heap_object<std::vector<EnvironmentDef>> envDef;
    ~EnvVarRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct ShellCommandRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<ShellCommandRequirement_class_ShellCommandRequirement_class> class_;
    ~ShellCommandRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct ResourceRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
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
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct WorkReuse
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<WorkReuse_class_WorkReuse_class> class_;
    heap_object<std::variant<bool, cwl_expression_string>> enableReuse;
    ~WorkReuse() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct NetworkAccess
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<NetworkAccess_class_NetworkAccess_class> class_;
    heap_object<std::variant<bool, cwl_expression_string>> networkAccess;
    ~NetworkAccess() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct InplaceUpdateRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<InplaceUpdateRequirement_class_InplaceUpdateRequirement_class> class_;
    heap_object<bool> inplaceUpdate;
    ~InplaceUpdateRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct ToolTimeLimit
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<ToolTimeLimit_class_ToolTimeLimit_class> class_;
    heap_object<std::variant<int32_t, int64_t, cwl_expression_string>> timelimit;
    ~ToolTimeLimit() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct ExpressionToolOutputParameter
    : https___w3id_org_cwl_cwl::OutputParameter {
    heap_object<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string, std::vector<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string>>>> type;
    ~ExpressionToolOutputParameter() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct WorkflowInputParameter
    : https___w3id_org_cwl_cwl::InputParameter {
    heap_object<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string, std::vector<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string>>>> type;
    heap_object<std::variant<std::monostate, InputBinding>> inputBinding;
    ~WorkflowInputParameter() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
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
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct WorkflowOutputParameter
    : https___w3id_org_cwl_cwl::OutputParameter {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> outputSource;
    heap_object<std::variant<std::monostate, LinkMergeMethod>> linkMerge;
    heap_object<std::variant<std::monostate, PickValueMethod>> pickValue;
    heap_object<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string, std::vector<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string>>>> type;
    ~WorkflowOutputParameter() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct Sink {
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> source;
    heap_object<std::variant<std::monostate, LinkMergeMethod>> linkMerge;
    heap_object<std::variant<std::monostate, PickValueMethod>> pickValue;
    virtual ~Sink() = 0;
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct WorkflowStepInput
    : https___w3id_org_cwl_cwl::Identified
    , https___w3id_org_cwl_cwl::Sink
    , https___w3id_org_cwl_cwl::LoadContents
    , https___w3id_org_cwl_cwl::Labeled {
    heap_object<std::variant<std::monostate, File, Directory, std::any>> default_;
    heap_object<std::variant<std::monostate, std::string, cwl_expression_string>> valueFrom;
    ~WorkflowStepInput() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct WorkflowStepOutput
    : https___w3id_org_cwl_cwl::Identified {
    ~WorkflowStepOutput() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct WorkflowStep
    : https___w3id_org_cwl_cwl::Identified
    , https___w3id_org_cwl_cwl::Labeled
    , https___w3id_org_cwl_salad::Documented {
    heap_object<std::vector<WorkflowStepInput>> in;
    heap_object<std::vector<std::variant<std::string, WorkflowStepOutput>>> out;
    heap_object<std::variant<std::monostate, std::vector<std::variant<InlineJavascriptRequirement, SchemaDefRequirement, LoadListingRequirement, DockerRequirement, SoftwareRequirement, InitialWorkDirRequirement, EnvVarRequirement, ShellCommandRequirement, ResourceRequirement, WorkReuse, NetworkAccess, InplaceUpdateRequirement, ToolTimeLimit, SubworkflowFeatureRequirement, ScatterFeatureRequirement, MultipleInputFeatureRequirement, StepInputExpressionRequirement>>>> requirements;
    heap_object<std::variant<std::monostate, std::vector<std::any>>> hints;
    heap_object<std::variant<std::string, CommandLineTool, ExpressionTool, Workflow, Operation>> run;
    heap_object<std::variant<std::monostate, cwl_expression_string>> when;
    heap_object<std::variant<std::monostate, std::string, std::vector<std::string>>> scatter;
    heap_object<std::variant<std::monostate, ScatterMethod>> scatterMethod;
    ~WorkflowStep() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
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
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

namespace https___w3id_org_cwl_cwl {
struct SubworkflowFeatureRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<SubworkflowFeatureRequirement_class_SubworkflowFeatureRequirement_class> class_;
    ~SubworkflowFeatureRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct ScatterFeatureRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<ScatterFeatureRequirement_class_ScatterFeatureRequirement_class> class_;
    ~ScatterFeatureRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct MultipleInputFeatureRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<MultipleInputFeatureRequirement_class_MultipleInputFeatureRequirement_class> class_;
    ~MultipleInputFeatureRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct StepInputExpressionRequirement
    : https___w3id_org_cwl_cwl::ProcessRequirement {
    heap_object<StepInputExpressionRequirement_class_StepInputExpressionRequirement_class> class_;
    ~StepInputExpressionRequirement() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct OperationInputParameter
    : https___w3id_org_cwl_cwl::InputParameter {
    heap_object<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string, std::vector<std::variant<CWLType, InputRecordSchema, InputEnumSchema, InputArraySchema, std::string>>>> type;
    ~OperationInputParameter() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
struct OperationOutputParameter
    : https___w3id_org_cwl_cwl::OutputParameter {
    heap_object<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string, std::vector<std::variant<CWLType, OutputRecordSchema, OutputEnumSchema, OutputArraySchema, std::string>>>> type;
    ~OperationOutputParameter() override = default;
    auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node override;
    void fromYaml(YAML::Node const& n) override;
};
}

namespace https___w3id_org_cwl_cwl {
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
    virtual auto toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node;
    virtual void fromYaml(YAML::Node const& n);
};
}

template <typename T>
heap_object<T>::~heap_object() = default;

inline https___w3id_org_cwl_salad::Documented::~Documented() = default;
inline auto https___w3id_org_cwl_salad::Documented::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_salad::Documented::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["doc"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *doc);
    }
}
inline auto https___w3id_org_cwl_salad::RecordField::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("RecordField");
    }
    n = mergeYaml(n, https___w3id_org_cwl_salad::Documented::toYaml(config));
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
inline void https___w3id_org_cwl_salad::RecordField::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_salad::Documented::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_salad::RecordField> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_salad::RecordField> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_salad::RecordField{};

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_salad::RecordSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_salad::RecordSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_salad::RecordSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_salad::RecordSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_salad::RecordSchema{};

        if constexpr (IsConstant<decltype(res.fields)::value_t>::value) try {
            fromYaml(n["fields"], *res.fields);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_salad::EnumSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_salad::EnumSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_salad::EnumSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_salad::EnumSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_salad::EnumSchema{};

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.symbols)::value_t>::value) try {
            fromYaml(n["symbols"], *res.symbols);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_salad::ArraySchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_salad::ArraySchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_salad::ArraySchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_salad::ArraySchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_salad::ArraySchema{};

        if constexpr (IsConstant<decltype(res.items)::value_t>::value) try {
            fromYaml(n["items"], *res.items);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::File::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::File::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::File> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::File> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::File{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.location)::value_t>::value) try {
            fromYaml(n["location"], *res.location);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.path)::value_t>::value) try {
            fromYaml(n["path"], *res.path);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.basename)::value_t>::value) try {
            fromYaml(n["basename"], *res.basename);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.dirname)::value_t>::value) try {
            fromYaml(n["dirname"], *res.dirname);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.nameroot)::value_t>::value) try {
            fromYaml(n["nameroot"], *res.nameroot);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.nameext)::value_t>::value) try {
            fromYaml(n["nameext"], *res.nameext);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.checksum)::value_t>::value) try {
            fromYaml(n["checksum"], *res.checksum);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.size)::value_t>::value) try {
            fromYaml(n["size"], *res.size);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.secondaryFiles)::value_t>::value) try {
            fromYaml(n["secondaryFiles"], *res.secondaryFiles);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.format)::value_t>::value) try {
            fromYaml(n["format"], *res.format);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.contents)::value_t>::value) try {
            fromYaml(n["contents"], *res.contents);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::Directory::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::Directory::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::Directory> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::Directory> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::Directory{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.location)::value_t>::value) try {
            fromYaml(n["location"], *res.location);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.path)::value_t>::value) try {
            fromYaml(n["path"], *res.path);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.basename)::value_t>::value) try {
            fromYaml(n["basename"], *res.basename);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.listing)::value_t>::value) try {
            fromYaml(n["listing"], *res.listing);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline https___w3id_org_cwl_cwl::Labeled::~Labeled() = default;
inline auto https___w3id_org_cwl_cwl::Labeled::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::Labeled::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["label"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *label);
    }
}
inline https___w3id_org_cwl_cwl::Identified::~Identified() = default;
inline auto https___w3id_org_cwl_cwl::Identified::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::Identified::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["id"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *id);
    }
}
inline https___w3id_org_cwl_cwl::LoadContents::~LoadContents() = default;
inline auto https___w3id_org_cwl_cwl::LoadContents::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::LoadContents::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
inline https___w3id_org_cwl_cwl::FieldBase::~FieldBase() = default;
inline auto https___w3id_org_cwl_cwl::FieldBase::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("FieldBase");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Labeled::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::FieldBase::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::Labeled::fromYaml(n);
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
inline https___w3id_org_cwl_cwl::InputFormat::~InputFormat() = default;
inline auto https___w3id_org_cwl_cwl::InputFormat::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::InputFormat::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["format"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *format);
    }
}
inline https___w3id_org_cwl_cwl::OutputFormat::~OutputFormat() = default;
inline auto https___w3id_org_cwl_cwl::OutputFormat::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::OutputFormat::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["format"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *format);
    }
}
inline https___w3id_org_cwl_cwl::Parameter::~Parameter() = default;
inline auto https___w3id_org_cwl_cwl::Parameter::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("Parameter");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::FieldBase::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_salad::Documented::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Identified::toYaml(config));
    return n;
}
inline void https___w3id_org_cwl_cwl::Parameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::FieldBase::fromYaml(n);
    https___w3id_org_cwl_salad::Documented::fromYaml(n);
    https___w3id_org_cwl_cwl::Identified::fromYaml(n);
}
inline auto https___w3id_org_cwl_cwl::InputBinding::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::InputBinding::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["loadContents"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *loadContents);
    }
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::InputBinding> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::InputBinding> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::InputBinding{};

        if constexpr (IsConstant<decltype(res.loadContents)::value_t>::value) try {
            fromYaml(n["loadContents"], *res.loadContents);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline https___w3id_org_cwl_cwl::IOSchema::~IOSchema() = default;
inline auto https___w3id_org_cwl_cwl::IOSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("IOSchema");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Labeled::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_salad::Documented::toYaml(config));
    {
         auto member = toYaml(*name, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "name", member);
    }
    return n;
}
inline void https___w3id_org_cwl_cwl::IOSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::Labeled::fromYaml(n);
    https___w3id_org_cwl_salad::Documented::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["name"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *name);
    }
}
inline https___w3id_org_cwl_cwl::InputSchema::~InputSchema() = default;
inline auto https___w3id_org_cwl_cwl::InputSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InputSchema");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::IOSchema::toYaml(config));
    return n;
}
inline void https___w3id_org_cwl_cwl::InputSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::IOSchema::fromYaml(n);
}
inline https___w3id_org_cwl_cwl::OutputSchema::~OutputSchema() = default;
inline auto https___w3id_org_cwl_cwl::OutputSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OutputSchema");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::IOSchema::toYaml(config));
    return n;
}
inline void https___w3id_org_cwl_cwl::OutputSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::IOSchema::fromYaml(n);
}
inline auto https___w3id_org_cwl_cwl::InputRecordField::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::InputRecordField::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::InputRecordField> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::InputRecordField> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::InputRecordField{};

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.secondaryFiles)::value_t>::value) try {
            fromYaml(n["secondaryFiles"], *res.secondaryFiles);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.streamable)::value_t>::value) try {
            fromYaml(n["streamable"], *res.streamable);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.format)::value_t>::value) try {
            fromYaml(n["format"], *res.format);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.loadContents)::value_t>::value) try {
            fromYaml(n["loadContents"], *res.loadContents);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.loadListing)::value_t>::value) try {
            fromYaml(n["loadListing"], *res.loadListing);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::InputRecordSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::InputRecordSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::InputRecordSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::InputRecordSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::InputRecordSchema{};

        if constexpr (IsConstant<decltype(res.fields)::value_t>::value) try {
            fromYaml(n["fields"], *res.fields);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::InputEnumSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InputEnumSchema");
    }
    n = mergeYaml(n, https___w3id_org_cwl_salad::EnumSchema::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_cwl::InputSchema::toYaml(config));
    return n;
}
inline void https___w3id_org_cwl_cwl::InputEnumSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_salad::EnumSchema::fromYaml(n);
    https___w3id_org_cwl_cwl::InputSchema::fromYaml(n);
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::InputEnumSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::InputEnumSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::InputEnumSchema{};

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::InputArraySchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::InputArraySchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::InputArraySchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::InputArraySchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::InputArraySchema{};

        if constexpr (IsConstant<decltype(res.items)::value_t>::value) try {
            fromYaml(n["items"], *res.items);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::OutputRecordField::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::OutputRecordField::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::OutputRecordField> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::OutputRecordField> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::OutputRecordField{};

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.secondaryFiles)::value_t>::value) try {
            fromYaml(n["secondaryFiles"], *res.secondaryFiles);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.streamable)::value_t>::value) try {
            fromYaml(n["streamable"], *res.streamable);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.format)::value_t>::value) try {
            fromYaml(n["format"], *res.format);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::OutputRecordSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::OutputRecordSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::OutputRecordSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::OutputRecordSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::OutputRecordSchema{};

        if constexpr (IsConstant<decltype(res.fields)::value_t>::value) try {
            fromYaml(n["fields"], *res.fields);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::OutputEnumSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OutputEnumSchema");
    }
    n = mergeYaml(n, https___w3id_org_cwl_salad::EnumSchema::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_cwl::OutputSchema::toYaml(config));
    return n;
}
inline void https___w3id_org_cwl_cwl::OutputEnumSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_salad::EnumSchema::fromYaml(n);
    https___w3id_org_cwl_cwl::OutputSchema::fromYaml(n);
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::OutputEnumSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::OutputEnumSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::OutputEnumSchema{};

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::OutputArraySchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::OutputArraySchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::OutputArraySchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::OutputArraySchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::OutputArraySchema{};

        if constexpr (IsConstant<decltype(res.items)::value_t>::value) try {
            fromYaml(n["items"], *res.items);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline https___w3id_org_cwl_cwl::InputParameter::~InputParameter() = default;
inline auto https___w3id_org_cwl_cwl::InputParameter::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InputParameter");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Parameter::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_cwl::InputFormat::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_cwl::LoadContents::toYaml(config));
    {
         auto member = toYaml(*default_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "default", member);
    }
    return n;
}
inline void https___w3id_org_cwl_cwl::InputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::Parameter::fromYaml(n);
    https___w3id_org_cwl_cwl::InputFormat::fromYaml(n);
    https___w3id_org_cwl_cwl::LoadContents::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["default"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *default_);
    }
}
inline https___w3id_org_cwl_cwl::OutputParameter::~OutputParameter() = default;
inline auto https___w3id_org_cwl_cwl::OutputParameter::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OutputParameter");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Parameter::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_cwl::OutputFormat::toYaml(config));
    return n;
}
inline void https___w3id_org_cwl_cwl::OutputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::Parameter::fromYaml(n);
    https___w3id_org_cwl_cwl::OutputFormat::fromYaml(n);
}
inline https___w3id_org_cwl_cwl::ProcessRequirement::~ProcessRequirement() = default;
inline auto https___w3id_org_cwl_cwl::ProcessRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ProcessRequirement");
    }
    return n;
}
inline void https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
}
inline https___w3id_org_cwl_cwl::Process::~Process() = default;
inline auto https___w3id_org_cwl_cwl::Process::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("Process");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Identified::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Labeled::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_salad::Documented::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::Process::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::Identified::fromYaml(n);
    https___w3id_org_cwl_cwl::Labeled::fromYaml(n);
    https___w3id_org_cwl_salad::Documented::fromYaml(n);
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
inline auto https___w3id_org_cwl_cwl::InlineJavascriptRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InlineJavascriptRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::InlineJavascriptRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::InlineJavascriptRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::InlineJavascriptRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::InlineJavascriptRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.expressionLib)::value_t>::value) try {
            fromYaml(n["expressionLib"], *res.expressionLib);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline https___w3id_org_cwl_cwl::CommandInputSchema::~CommandInputSchema() = default;
inline auto https___w3id_org_cwl_cwl::CommandInputSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandInputSchema");
    }
    return n;
}
inline void https___w3id_org_cwl_cwl::CommandInputSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
}
inline auto https___w3id_org_cwl_cwl::SchemaDefRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("SchemaDefRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::SchemaDefRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::SchemaDefRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::SchemaDefRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::SchemaDefRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.types)::value_t>::value) try {
            fromYaml(n["types"], *res.types);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::SecondaryFileSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::SecondaryFileSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::SecondaryFileSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::SecondaryFileSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::SecondaryFileSchema{};

        if constexpr (IsConstant<decltype(res.pattern)::value_t>::value) try {
            fromYaml(n["pattern"], *res.pattern);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.required)::value_t>::value) try {
            fromYaml(n["required"], *res.required);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::LoadListingRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("LoadListingRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::LoadListingRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::LoadListingRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::LoadListingRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::LoadListingRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.loadListing)::value_t>::value) try {
            fromYaml(n["loadListing"], *res.loadListing);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::EnvironmentDef::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::EnvironmentDef::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::EnvironmentDef> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::EnvironmentDef> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::EnvironmentDef{};

        if constexpr (IsConstant<decltype(res.envName)::value_t>::value) try {
            fromYaml(n["envName"], *res.envName);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.envValue)::value_t>::value) try {
            fromYaml(n["envValue"], *res.envValue);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandLineBinding::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandLineBinding");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::InputBinding::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::CommandLineBinding::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::InputBinding::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandLineBinding> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandLineBinding> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandLineBinding{};

        if constexpr (IsConstant<decltype(res.position)::value_t>::value) try {
            fromYaml(n["position"], *res.position);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.prefix)::value_t>::value) try {
            fromYaml(n["prefix"], *res.prefix);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.separate)::value_t>::value) try {
            fromYaml(n["separate"], *res.separate);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.itemSeparator)::value_t>::value) try {
            fromYaml(n["itemSeparator"], *res.itemSeparator);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.valueFrom)::value_t>::value) try {
            fromYaml(n["valueFrom"], *res.valueFrom);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.shellQuote)::value_t>::value) try {
            fromYaml(n["shellQuote"], *res.shellQuote);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandOutputBinding::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandOutputBinding");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::LoadContents::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::CommandOutputBinding::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::LoadContents::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandOutputBinding> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandOutputBinding> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandOutputBinding{};

        if constexpr (IsConstant<decltype(res.glob)::value_t>::value) try {
            fromYaml(n["glob"], *res.glob);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.outputEval)::value_t>::value) try {
            fromYaml(n["outputEval"], *res.outputEval);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandLineBindable::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::CommandLineBindable::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    {
        auto nodeAsList = convertMapToList(n["inputBinding"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *inputBinding);
    }
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandLineBindable> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandLineBindable> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandLineBindable{};

        if constexpr (IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandInputRecordField::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::CommandInputRecordField::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandInputRecordField> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandInputRecordField> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandInputRecordField{};

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.secondaryFiles)::value_t>::value) try {
            fromYaml(n["secondaryFiles"], *res.secondaryFiles);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.streamable)::value_t>::value) try {
            fromYaml(n["streamable"], *res.streamable);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.format)::value_t>::value) try {
            fromYaml(n["format"], *res.format);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.loadContents)::value_t>::value) try {
            fromYaml(n["loadContents"], *res.loadContents);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.loadListing)::value_t>::value) try {
            fromYaml(n["loadListing"], *res.loadListing);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandInputRecordSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::CommandInputRecordSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandInputRecordSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandInputRecordSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandInputRecordSchema{};

        if constexpr (IsConstant<decltype(res.fields)::value_t>::value) try {
            fromYaml(n["fields"], *res.fields);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandInputEnumSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::CommandInputEnumSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandInputEnumSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandInputEnumSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandInputEnumSchema{};

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.symbols)::value_t>::value) try {
            fromYaml(n["symbols"], *res.symbols);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandInputArraySchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::CommandInputArraySchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandInputArraySchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandInputArraySchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandInputArraySchema{};

        if constexpr (IsConstant<decltype(res.items)::value_t>::value) try {
            fromYaml(n["items"], *res.items);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandOutputRecordField::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::CommandOutputRecordField::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandOutputRecordField> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandOutputRecordField> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandOutputRecordField{};

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.secondaryFiles)::value_t>::value) try {
            fromYaml(n["secondaryFiles"], *res.secondaryFiles);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.streamable)::value_t>::value) try {
            fromYaml(n["streamable"], *res.streamable);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.format)::value_t>::value) try {
            fromYaml(n["format"], *res.format);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.outputBinding)::value_t>::value) try {
            fromYaml(n["outputBinding"], *res.outputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandOutputRecordSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::CommandOutputRecordSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandOutputRecordSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandOutputRecordSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandOutputRecordSchema{};

        if constexpr (IsConstant<decltype(res.fields)::value_t>::value) try {
            fromYaml(n["fields"], *res.fields);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandOutputEnumSchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::CommandOutputEnumSchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandOutputEnumSchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandOutputEnumSchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandOutputEnumSchema{};

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.symbols)::value_t>::value) try {
            fromYaml(n["symbols"], *res.symbols);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandOutputArraySchema::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::CommandOutputArraySchema::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandOutputArraySchema> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandOutputArraySchema> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandOutputArraySchema{};

        if constexpr (IsConstant<decltype(res.items)::value_t>::value) try {
            fromYaml(n["items"], *res.items);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.name)::value_t>::value) try {
            fromYaml(n["name"], *res.name);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandInputParameter::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandInputParameter");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::InputParameter::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::CommandInputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::InputParameter::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandInputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandInputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandInputParameter{};

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandOutputParameter::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("CommandOutputParameter");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::OutputParameter::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::CommandOutputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::OutputParameter::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandOutputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandOutputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandOutputParameter{};

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.outputBinding)::value_t>::value) try {
            fromYaml(n["outputBinding"], *res.outputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::CommandLineTool::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::CommandLineTool::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::CommandLineTool> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::CommandLineTool> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::CommandLineTool{};

        if constexpr (IsConstant<decltype(res.id)::value_t>::value) try {
            fromYaml(n["id"], *res.id);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.inputs)::value_t>::value) try {
            fromYaml(n["inputs"], *res.inputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.outputs)::value_t>::value) try {
            fromYaml(n["outputs"], *res.outputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.requirements)::value_t>::value) try {
            fromYaml(n["requirements"], *res.requirements);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.hints)::value_t>::value) try {
            fromYaml(n["hints"], *res.hints);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.cwlVersion)::value_t>::value) try {
            fromYaml(n["cwlVersion"], *res.cwlVersion);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.intent)::value_t>::value) try {
            fromYaml(n["intent"], *res.intent);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.baseCommand)::value_t>::value) try {
            fromYaml(n["baseCommand"], *res.baseCommand);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.arguments)::value_t>::value) try {
            fromYaml(n["arguments"], *res.arguments);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.stdin_)::value_t>::value) try {
            fromYaml(n["stdin"], *res.stdin_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.stderr_)::value_t>::value) try {
            fromYaml(n["stderr"], *res.stderr_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.stdout_)::value_t>::value) try {
            fromYaml(n["stdout"], *res.stdout_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.successCodes)::value_t>::value) try {
            fromYaml(n["successCodes"], *res.successCodes);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.temporaryFailCodes)::value_t>::value) try {
            fromYaml(n["temporaryFailCodes"], *res.temporaryFailCodes);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.permanentFailCodes)::value_t>::value) try {
            fromYaml(n["permanentFailCodes"], *res.permanentFailCodes);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::DockerRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("DockerRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::DockerRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::DockerRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::DockerRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::DockerRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.dockerPull)::value_t>::value) try {
            fromYaml(n["dockerPull"], *res.dockerPull);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.dockerLoad)::value_t>::value) try {
            fromYaml(n["dockerLoad"], *res.dockerLoad);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.dockerFile)::value_t>::value) try {
            fromYaml(n["dockerFile"], *res.dockerFile);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.dockerImport)::value_t>::value) try {
            fromYaml(n["dockerImport"], *res.dockerImport);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.dockerImageId)::value_t>::value) try {
            fromYaml(n["dockerImageId"], *res.dockerImageId);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.dockerOutputDirectory)::value_t>::value) try {
            fromYaml(n["dockerOutputDirectory"], *res.dockerOutputDirectory);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::SoftwareRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("SoftwareRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::SoftwareRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::SoftwareRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::SoftwareRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::SoftwareRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.packages)::value_t>::value) try {
            fromYaml(n["packages"], *res.packages);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::SoftwarePackage::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::SoftwarePackage::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::SoftwarePackage> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::SoftwarePackage> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::SoftwarePackage{};

        if constexpr (IsConstant<decltype(res.package)::value_t>::value) try {
            fromYaml(n["package"], *res.package);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.version)::value_t>::value) try {
            fromYaml(n["version"], *res.version);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.specs)::value_t>::value) try {
            fromYaml(n["specs"], *res.specs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::Dirent::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::Dirent::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::Dirent> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::Dirent> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::Dirent{};

        if constexpr (IsConstant<decltype(res.entryname)::value_t>::value) try {
            fromYaml(n["entryname"], *res.entryname);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.entry)::value_t>::value) try {
            fromYaml(n["entry"], *res.entry);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.writable)::value_t>::value) try {
            fromYaml(n["writable"], *res.writable);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::InitialWorkDirRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InitialWorkDirRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::InitialWorkDirRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::InitialWorkDirRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::InitialWorkDirRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::InitialWorkDirRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.listing)::value_t>::value) try {
            fromYaml(n["listing"], *res.listing);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::EnvVarRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("EnvVarRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::EnvVarRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::EnvVarRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::EnvVarRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::EnvVarRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.envDef)::value_t>::value) try {
            fromYaml(n["envDef"], *res.envDef);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::ShellCommandRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ShellCommandRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    return n;
}
inline void https___w3id_org_cwl_cwl::ShellCommandRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::ShellCommandRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::ShellCommandRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::ShellCommandRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::ResourceRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ResourceRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::ResourceRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::ResourceRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::ResourceRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::ResourceRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.coresMin)::value_t>::value) try {
            fromYaml(n["coresMin"], *res.coresMin);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.coresMax)::value_t>::value) try {
            fromYaml(n["coresMax"], *res.coresMax);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.ramMin)::value_t>::value) try {
            fromYaml(n["ramMin"], *res.ramMin);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.ramMax)::value_t>::value) try {
            fromYaml(n["ramMax"], *res.ramMax);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.tmpdirMin)::value_t>::value) try {
            fromYaml(n["tmpdirMin"], *res.tmpdirMin);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.tmpdirMax)::value_t>::value) try {
            fromYaml(n["tmpdirMax"], *res.tmpdirMax);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.outdirMin)::value_t>::value) try {
            fromYaml(n["outdirMin"], *res.outdirMin);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.outdirMax)::value_t>::value) try {
            fromYaml(n["outdirMax"], *res.outdirMax);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::WorkReuse::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("WorkReuse");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::WorkReuse::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::WorkReuse> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::WorkReuse> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::WorkReuse{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.enableReuse)::value_t>::value) try {
            fromYaml(n["enableReuse"], *res.enableReuse);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::NetworkAccess::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("NetworkAccess");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::NetworkAccess::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::NetworkAccess> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::NetworkAccess> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::NetworkAccess{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.networkAccess)::value_t>::value) try {
            fromYaml(n["networkAccess"], *res.networkAccess);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::InplaceUpdateRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("InplaceUpdateRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::InplaceUpdateRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::InplaceUpdateRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::InplaceUpdateRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::InplaceUpdateRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.inplaceUpdate)::value_t>::value) try {
            fromYaml(n["inplaceUpdate"], *res.inplaceUpdate);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::ToolTimeLimit::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ToolTimeLimit");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::ToolTimeLimit::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::ToolTimeLimit> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::ToolTimeLimit> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::ToolTimeLimit{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.timelimit)::value_t>::value) try {
            fromYaml(n["timelimit"], *res.timelimit);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::ExpressionToolOutputParameter::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ExpressionToolOutputParameter");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::OutputParameter::toYaml(config));
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    return n;
}
inline void https___w3id_org_cwl_cwl::ExpressionToolOutputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::OutputParameter::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::ExpressionToolOutputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::ExpressionToolOutputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::ExpressionToolOutputParameter{};

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::WorkflowInputParameter::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("WorkflowInputParameter");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::InputParameter::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::WorkflowInputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::InputParameter::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::WorkflowInputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::WorkflowInputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::WorkflowInputParameter{};

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.inputBinding)::value_t>::value) try {
            fromYaml(n["inputBinding"], *res.inputBinding);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::ExpressionTool::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::ExpressionTool::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::ExpressionTool> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::ExpressionTool> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::ExpressionTool{};

        if constexpr (IsConstant<decltype(res.id)::value_t>::value) try {
            fromYaml(n["id"], *res.id);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.inputs)::value_t>::value) try {
            fromYaml(n["inputs"], *res.inputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.outputs)::value_t>::value) try {
            fromYaml(n["outputs"], *res.outputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.requirements)::value_t>::value) try {
            fromYaml(n["requirements"], *res.requirements);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.hints)::value_t>::value) try {
            fromYaml(n["hints"], *res.hints);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.cwlVersion)::value_t>::value) try {
            fromYaml(n["cwlVersion"], *res.cwlVersion);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.intent)::value_t>::value) try {
            fromYaml(n["intent"], *res.intent);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.expression)::value_t>::value) try {
            fromYaml(n["expression"], *res.expression);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::WorkflowOutputParameter::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("WorkflowOutputParameter");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::OutputParameter::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::WorkflowOutputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::OutputParameter::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::WorkflowOutputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::WorkflowOutputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::WorkflowOutputParameter{};

        if constexpr (IsConstant<decltype(res.outputSource)::value_t>::value) try {
            fromYaml(n["outputSource"], *res.outputSource);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.linkMerge)::value_t>::value) try {
            fromYaml(n["linkMerge"], *res.linkMerge);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.pickValue)::value_t>::value) try {
            fromYaml(n["pickValue"], *res.pickValue);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline https___w3id_org_cwl_cwl::Sink::~Sink() = default;
inline auto https___w3id_org_cwl_cwl::Sink::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::Sink::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
inline auto https___w3id_org_cwl_cwl::WorkflowStepInput::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("WorkflowStepInput");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Identified::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Sink::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_cwl::LoadContents::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Labeled::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::WorkflowStepInput::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::Identified::fromYaml(n);
    https___w3id_org_cwl_cwl::Sink::fromYaml(n);
    https___w3id_org_cwl_cwl::LoadContents::fromYaml(n);
    https___w3id_org_cwl_cwl::Labeled::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::WorkflowStepInput> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::WorkflowStepInput> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::WorkflowStepInput{};

        if constexpr (IsConstant<decltype(res.default_)::value_t>::value) try {
            fromYaml(n["default"], *res.default_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.valueFrom)::value_t>::value) try {
            fromYaml(n["valueFrom"], *res.valueFrom);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::WorkflowStepOutput::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("WorkflowStepOutput");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Identified::toYaml(config));
    return n;
}
inline void https___w3id_org_cwl_cwl::WorkflowStepOutput::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::Identified::fromYaml(n);
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::WorkflowStepOutput> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::WorkflowStepOutput> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::WorkflowStepOutput{};

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::WorkflowStep::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("WorkflowStep");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Identified::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_cwl::Labeled::toYaml(config));
    n = mergeYaml(n, https___w3id_org_cwl_salad::Documented::toYaml(config));
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
inline void https___w3id_org_cwl_cwl::WorkflowStep::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::Identified::fromYaml(n);
    https___w3id_org_cwl_cwl::Labeled::fromYaml(n);
    https___w3id_org_cwl_salad::Documented::fromYaml(n);
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::WorkflowStep> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::WorkflowStep> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::WorkflowStep{};

        if constexpr (IsConstant<decltype(res.in)::value_t>::value) try {
            fromYaml(n["in"], *res.in);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.out)::value_t>::value) try {
            fromYaml(n["out"], *res.out);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.requirements)::value_t>::value) try {
            fromYaml(n["requirements"], *res.requirements);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.hints)::value_t>::value) try {
            fromYaml(n["hints"], *res.hints);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.run)::value_t>::value) try {
            fromYaml(n["run"], *res.run);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.when)::value_t>::value) try {
            fromYaml(n["when"], *res.when);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.scatter)::value_t>::value) try {
            fromYaml(n["scatter"], *res.scatter);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.scatterMethod)::value_t>::value) try {
            fromYaml(n["scatterMethod"], *res.scatterMethod);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::Workflow::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::Workflow::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::Workflow> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::Workflow> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::Workflow{};

        if constexpr (IsConstant<decltype(res.id)::value_t>::value) try {
            fromYaml(n["id"], *res.id);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.inputs)::value_t>::value) try {
            fromYaml(n["inputs"], *res.inputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.outputs)::value_t>::value) try {
            fromYaml(n["outputs"], *res.outputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.requirements)::value_t>::value) try {
            fromYaml(n["requirements"], *res.requirements);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.hints)::value_t>::value) try {
            fromYaml(n["hints"], *res.hints);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.cwlVersion)::value_t>::value) try {
            fromYaml(n["cwlVersion"], *res.cwlVersion);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.intent)::value_t>::value) try {
            fromYaml(n["intent"], *res.intent);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.steps)::value_t>::value) try {
            fromYaml(n["steps"], *res.steps);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("SubworkflowFeatureRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    return n;
}
inline void https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::SubworkflowFeatureRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::ScatterFeatureRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("ScatterFeatureRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    return n;
}
inline void https___w3id_org_cwl_cwl::ScatterFeatureRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::ScatterFeatureRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::ScatterFeatureRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::ScatterFeatureRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("MultipleInputFeatureRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    return n;
}
inline void https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::MultipleInputFeatureRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::StepInputExpressionRequirement::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("StepInputExpressionRequirement");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::ProcessRequirement::toYaml(config));
    {
         auto member = toYaml(*class_, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "class", member);
    }
    return n;
}
inline void https___w3id_org_cwl_cwl::StepInputExpressionRequirement::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::ProcessRequirement::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["class"], "", "");
        auto expandedNode = (nodeAsList);
        fromYaml(expandedNode, *class_);
    }
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::StepInputExpressionRequirement> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::StepInputExpressionRequirement> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::StepInputExpressionRequirement{};

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::OperationInputParameter::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OperationInputParameter");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::InputParameter::toYaml(config));
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    return n;
}
inline void https___w3id_org_cwl_cwl::OperationInputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::InputParameter::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::OperationInputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::OperationInputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::OperationInputParameter{};

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::OperationOutputParameter::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
    auto n = YAML::Node{};
    if (config.generateTags) {
        n.SetTag("OperationOutputParameter");
    }
    n = mergeYaml(n, https___w3id_org_cwl_cwl::OutputParameter::toYaml(config));
    {
         auto member = toYaml(*type, config);
         member = simplifyType(member, config);
         member = convertListToMap(member, "", "", config);
        addYamlField(n, "type", member);
    }
    return n;
}
inline void https___w3id_org_cwl_cwl::OperationOutputParameter::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
    https___w3id_org_cwl_cwl::OutputParameter::fromYaml(n);
    {
        auto nodeAsList = convertMapToList(n["type"], "", "");
        auto expandedNode = expandType(nodeAsList);
        fromYaml(expandedNode, *type);
    }
}
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::OperationOutputParameter> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::OperationOutputParameter> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::OperationOutputParameter{};

        if constexpr (IsConstant<decltype(res.type)::value_t>::value) try {
            fromYaml(n["type"], *res.type);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};
inline auto https___w3id_org_cwl_cwl::Operation::toYaml([[maybe_unused]] store_config const& config) const -> YAML::Node {
    using ::cpp_gen::toYaml;
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
inline void https___w3id_org_cwl_cwl::Operation::fromYaml([[maybe_unused]] YAML::Node const& n) {
    using ::cpp_gen::fromYaml;
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
template <>
struct DetectAndExtractFromYaml<https___w3id_org_cwl_cwl::Operation> {
    auto operator()(YAML::Node const& n) const -> std::optional<https___w3id_org_cwl_cwl::Operation> {
        if (!n.IsDefined()) return std::nullopt;
        if (!n.IsMap()) return std::nullopt;
        auto res = https___w3id_org_cwl_cwl::Operation{};

        if constexpr (IsConstant<decltype(res.id)::value_t>::value) try {
            fromYaml(n["id"], *res.id);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.label)::value_t>::value) try {
            fromYaml(n["label"], *res.label);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.doc)::value_t>::value) try {
            fromYaml(n["doc"], *res.doc);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.inputs)::value_t>::value) try {
            fromYaml(n["inputs"], *res.inputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.outputs)::value_t>::value) try {
            fromYaml(n["outputs"], *res.outputs);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.requirements)::value_t>::value) try {
            fromYaml(n["requirements"], *res.requirements);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.hints)::value_t>::value) try {
            fromYaml(n["hints"], *res.hints);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.cwlVersion)::value_t>::value) try {
            fromYaml(n["cwlVersion"], *res.cwlVersion);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.intent)::value_t>::value) try {
            fromYaml(n["intent"], *res.intent);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        if constexpr (IsConstant<decltype(res.class_)::value_t>::value) try {
            fromYaml(n["class"], *res.class_);
            fromYaml(n, res);
            return res;
        } catch(...) {}

        return std::nullopt;
    }
};

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
using DocumentRootType = std::variant<https___w3id_org_cwl_cwl::CommandLineTool, https___w3id_org_cwl_cwl::ExpressionTool, https___w3id_org_cwl_cwl::Workflow, https___w3id_org_cwl_cwl::Operation>;
auto load_document_from_yaml(YAML::Node n) -> DocumentRootType {
    DocumentRootType root;
    fromYaml(n, root);
    return root;
}
auto load_document_from_string(std::string document) -> DocumentRootType {
    return load_document_from_yaml(YAML::Load(document));
}
auto load_document(std::filesystem::path path) -> DocumentRootType {
    return load_document_from_yaml(YAML::LoadFile(path));
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