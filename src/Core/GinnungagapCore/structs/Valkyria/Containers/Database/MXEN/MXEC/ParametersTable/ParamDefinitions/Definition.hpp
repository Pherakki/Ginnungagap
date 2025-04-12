#pragma once

#include <string>
#include <GinnungagapCore/structs/JSON/load.hpp>



struct MXEParameterSetGroupDefinition;
struct MXEEntityDefinition;
struct MXEFlatEntityDefinition;

using ParameterDefMap  = std::unordered_map<std::string, MXEParameterSetGroupDefinition>;
using EntityDefMap     = std::unordered_map<std::string, MXEEntityDefinition>;
using FlatEntityDefMap = std::unordered_map<std::string, MXEFlatEntityDefinition>;

enum class ElementType : uint8_t
{
    Int8=0,
    Int16=1,
    Int32=2,
    Int64=3,
    UInt8=4,
    UInt16=5,
    UInt32=6,
    UInt64=7,
    Pad8=8,
    Pad16=9,
    Pad32=10,
    Pad64=11,
    Asset=12,
    Offset32=13,
    Offset64=14,
    Float32=15,
    Float64=16,
    SJISString=17,
    UTF8String=18,
    CharColor=19,
    FloatColor=20
};

template<typename T>
struct OptionalMember
{
    bool exists=false;
    T data;
};

struct MXESubParameterDefinition
{
    std::string name;
    std::string type;
    size_t count;
    size_t offset;
};

struct DataElementDefinition
{
    std::string name;
    std::string label;
    ElementType type;
    std::string type_name;
    std::string endianness;
};

struct SubParameters
{
protected:
    std::unordered_map<std::string, size_t> subparam_indices;
    std::vector<MXESubParameterDefinition> subparams;
public:
    MXESubParameterDefinition& at(const std::string& key)
    {
        return (*this)[subparam_indices.at(key)];
    }

    const MXESubParameterDefinition& at(const std::string& key) const
    {
        return (*this)[subparam_indices.at(key)];
    }

    MXESubParameterDefinition& operator[](size_t index)
    {
        return subparams[index];
    }

    const MXESubParameterDefinition& operator[](size_t index) const
    {
        return subparams[index];
    }

    MXESubParameterDefinition& addDefinition(const std::string& key)
    {
        this->subparam_indices.insert({key, this->subparams.size()});
        return this->subparams.emplace_back();
    }

    size_t         size  () const { return this->subparams.size();  }
    bool           empty () const { return this->subparams.empty(); }
    constexpr auto begin ()       { return this->subparams.begin(); }
    constexpr auto end   ()       { return this->subparams.end  (); }
    constexpr auto rbegin()       { return this->subparams.begin(); }
    constexpr auto rend  ()       { return this->subparams.end  (); }
    constexpr auto begin () const { return this->subparams.begin(); }
    constexpr auto end   () const { return this->subparams.end  (); }
    constexpr auto rbegin() const { return this->subparams.begin(); }
    constexpr auto rend  () const { return this->subparams.end  (); }
};

struct MXEParameterSetDefinition
{
    size_t size;
    std::vector<DataElementDefinition> data;
    std::map<size_t, std::string>      assets;
    SubParameters                      subparams;

    std::vector<size_t> sjis_vars;
    std::vector<size_t> utf8_vars;
    std::vector<size_t> asset_vars;
    std::vector<size_t> asset_offsets;
    std::unordered_map<std::string, size_t> name_to_idx;

    // Roles
    std::vector<size_t> position;
    std::vector<size_t> rotation;
    std::vector<size_t> scale;
    size_t              team = 0xFFFFFFFFFFFFFFFF;

    MXEParameterSetDefinition() = default;
    MXEParameterSetDefinition(const nlohmann::ordered_json& json, const std::string& name);
};

struct MXEParameterSetGroupDefinition
{
    std::string name;
    std::map<size_t, MXEParameterSetDefinition> group;
    MXEParameterSetDefinition default_def;

    MXEParameterSetGroupDefinition(const std::string& name, const nlohmann::ordered_json& json);
    const MXEParameterSetDefinition& getDefinition(size_t size) const;
    const MXEParameterSetDefinition& fullDefinition() const;
    bool  hasMultipleDefinitions() const;
    std::vector<size_t> getDefinitionSizes() const;
};


struct MXEEntityParameter
{
    std::string member_type;
    std::string param_type;
};


struct MXEEntityComponent
{
    std::string name;
    std::vector<MXEEntityParameter> parameters;
};


struct MXEEntityDefinition
{
    MXEEntityComponent component;
    std::vector<std::string> subcomponents;

    MXEEntityDefinition(const std::string& name, const nlohmann::ordered_json& json);
};


struct MXEFlatEntityComponent
{
    int parent;
    int next_sibling;
    std::string name;
    std::string full_name;
    std::vector<MXEEntityParameter> parameters;

    MXEFlatEntityComponent(const MXEEntityComponent& component, int parent=-1, int next_sibling=-1, const std::string& parent_name="");
};


struct MXEFlatEntityDefinition
{
    std::string name;
    std::vector<MXEFlatEntityComponent> components;

    MXEFlatEntityDefinition(const MXEEntityDefinition& component, const EntityDefMap& defmap);
    MXEFlatEntityDefinition(const std::string& name, const EntityDefMap& defmap);
};
