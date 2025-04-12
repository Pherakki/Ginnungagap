#include "Definition.hpp"
#include <iostream>


bool isUInt(const std::string& v)
{
    return v.find_first_not_of("0123456789") == v.npos;
}

MXEParameterSetDefinition::MXEParameterSetDefinition(const nlohmann::ordered_json& json, const std::string& name)
{
    if (json.contains("struct"))
    {
        size=0;
        const auto& structdef = json.at("struct");
        if (structdef.is_array())
        {
            size_t idx=0;
            for (const auto& param_chunk : json.at("struct"))
            {
                if (!param_chunk.is_structured()) continue;

                for(auto it = param_chunk.begin(); it != param_chunk.end(); ++it)
                {
                    std::string str_dtype;
                    std::string label;
                    std::string endianness;
                    if (it.value().is_structured())
                    {
                        const auto& elem_def = it.value();

                        for (auto ed_it = elem_def.begin(); ed_it != elem_def.end(); ++ed_it)
                        {
                            if (!ed_it.value().is_string())
                                continue;
                            
                            if (ed_it.key() == "label")
                            {
                                label = ed_it.value();
                            }
                            else if (ed_it.key() == "type")
                            {
                                std::string v = ed_it.value();
                                endianness = v.substr(0, 1);
                                str_dtype = v.substr(1);
                            }
                        }
                    }
                    else if (it.value().is_string())
                    {
                        std::string v = it.value();
                        endianness = v.substr(0, 1);
                        str_dtype = v.substr(1);
                        label = it.key();
                    }
                    else
                    {
                        continue;
                    }

                    ElementType dtype;
                    
                    if (str_dtype == "int8")
                    {
                        size += 1;
                        dtype = ElementType::Int8;
                    }
                    else if (str_dtype == "int16")
                    {
                        size += 2;
                        dtype = ElementType::Int16;
                    }
                    else if (str_dtype == "int32")
                    {
                        size += 4;
                        dtype = ElementType::Int32;
                    }
                    else if (str_dtype == "int64")
                    {
                        size += 8;
                        dtype = ElementType::Int64;
                    }
                    else if (str_dtype == "uint8")
                    {
                        size += 1;
                        dtype = ElementType::UInt8;
                    }
                    else if (str_dtype == "uint16")
                    {
                        size += 2;
                        dtype = ElementType::UInt16;
                    }
                    else if (str_dtype == "uint32")
                    {
                        size += 4;
                        dtype = ElementType::UInt32;
                    }
                    else if (str_dtype == "uint64")
                    {
                        size += 8;
                        dtype = ElementType::UInt64;
                    }
                    else if (str_dtype == "pad8")
                    {
                        size += 1;
                        dtype = ElementType::Pad8;
                    }
                    else if (str_dtype == "pad16")
                    {
                        size += 2;
                        dtype = ElementType::Pad16;
                    }
                    else if (str_dtype == "pad32")
                    {
                        size += 4;
                        dtype = ElementType::Pad32;
                    }
                    else if (str_dtype == "pad64")
                    {
                        size += 8;
                        dtype = ElementType::Pad64;
                    }
                    else if (str_dtype == "asset")
                    {
                        this->asset_vars.push_back(idx);
                        this->asset_offsets.push_back(size);
                        size += 8;
                        dtype = ElementType::Asset;
                    }
                    else if (str_dtype == "pointer32")
                    {
                        size += 4;
                        dtype = ElementType::Offset32;
                    }
                    else if (str_dtype == "pointer64")
                    {
                        size += 8;
                        dtype = ElementType::Offset64;
                    }
                    else if (str_dtype == "offset32")
                    {
                        size += 4;
                        dtype = ElementType::Offset32;
                    }
                    else if (str_dtype == "offset64")
                    {
                        size += 8;
                        dtype = ElementType::Offset64;
                    }
                    else if (str_dtype == "float32")
                    {
                        size += 4;
                        dtype = ElementType::Float32;
                    }
                    else if (str_dtype == "float64")
                    {
                        size += 8;
                        dtype = ElementType::Float64;
                    }
                    else if (str_dtype == "sjis_string")
                    {
                        size += 8;
                        dtype = ElementType::SJISString;
                    }
                    else if (str_dtype == "utf8_string")
                    {
                        size += 8;
                        dtype = ElementType::UTF8String;
                    }
                    else if (str_dtype == "color32")
                    {
                        size += 4;
                        dtype = ElementType::CharColor;
                    }
                    else if (str_dtype == "color128")
                    {
                        size += 16;
                        dtype = ElementType::FloatColor;
                    }
                    else
                        throw std::runtime_error("Unknown parameter element type '" + str_dtype + "' in parameter definition " + name);

                    const auto& elem_def = this->data.emplace_back(DataElementDefinition{.name=it.key(), .label=label, .type=dtype, .type_name=str_dtype, .endianness=endianness});

                    if (elem_def.type == ElementType::SJISString)
                        this->sjis_vars.push_back(idx);
                    else if (elem_def.type == ElementType::UTF8String)
                        this->utf8_vars.push_back(idx);

                    this->name_to_idx.insert({it.key(), idx});
                    ++idx;
                }
            }
        }
    }

    if (json.contains("assets"))
    {
        const auto& assets = json.at("assets");
        if (assets.is_structured())
        {
            for(auto it = assets.begin(); it != assets.end(); ++it)
            {
                if (!it.value().is_string()) continue;
                if (!this->name_to_idx.contains(it.key()))
                    throw std::runtime_error("Structure definition does not contain the key '" + it.key() + "'");
                this->assets[this->name_to_idx.at(it.key())] = it.value();
            }
        }
    }

    if (json.contains("roles"))
    {
        const auto& roles = json.at("roles");
        if (roles.is_structured())
        {
            // Position
            if (roles.contains("position"))
            {
                const auto& position = roles.at("position");
                if (position.is_array())
                {
                    for (const auto& v : position)
                    {
                        if (!v.is_string())
                            throw std::runtime_error("'position' role definition contains non-string elements");
                        if (!this->name_to_idx.contains(v))
                            throw std::runtime_error("'" + v.get<std::string>() + "' used in 'position' definition is not defined in the structure");
                        this->position.push_back(this->name_to_idx.at(v));
                    }
                }
            }

            // Rotation
            if (roles.contains("rotation"))
            {
                const auto& rotation = roles.at("rotation");
                if (rotation.is_array())
                {
                    for (const auto& v : rotation)
                    {
                        if (!v.is_string())
                            throw std::runtime_error("'rotation' role definition contains non-string elements");
                        if (!this->name_to_idx.contains(v))
                            throw std::runtime_error("'" + v.get<std::string>() + "' used in 'rotation' definition is not defined in the structure");
                        this->rotation.push_back(this->name_to_idx.at(v));
                    }
                }
            }

            // Scale
            if (roles.contains("scale"))
            {
                const auto& scale = roles.at("scale");
                if (scale.is_array())
                {
                    for (const auto& v : scale)
                    {
                        if (!v.is_string())
                            throw std::runtime_error("'scale' role definition contains non-string elements");
                        if (!this->name_to_idx.contains(v))
                            throw std::runtime_error("'" + v.get<std::string>() + "' used in 'scale' definition is not defined in the structure");
                        this->scale.push_back(this->name_to_idx.at(v));
                    }
                }
            }

            // Team
            if (roles.contains("team"))
            {
                const auto& team = roles.at("team");
                if (team.is_string())
                    this->team = this->name_to_idx.at(team);
            }
        }
    }

    if (json.contains("subparams"))
    {
        const auto& subparams = json.at("subparams");
        if (subparams.is_structured())
        {
            for(auto it = subparams.begin(); it != subparams.end(); ++it)
            {
                const auto& name = it.key();
                const auto& spdef = it.value();
                if (!spdef.is_structured()) continue;

                if (!spdef.contains("type")) continue;
                auto& tdef = spdef.at("type");
                if (!tdef.is_string()) continue;
                std::string tval = tdef;

                if (!spdef.contains("count")) continue;
                auto& cdef = spdef.at("count");
                if (!cdef.is_string()) continue;
                std::string cval = cdef;
                if (!this->name_to_idx.contains(cval))
                    throw std::runtime_error("Count variable for subparameter definition '" + name + "' is not a member of the definition structure.");

                if (!spdef.contains("pointer")) continue;
                auto& pdef = spdef.at("pointer");
                if (!pdef.is_string()) continue;
                std::string pval = pdef;
                if (!this->name_to_idx.contains(pval))
                    throw std::runtime_error("Pointer variable for subparameter definition '" + name + "' is not a member of the definition structure.");

                auto& subparam_def = this->subparams.addDefinition(name);
                subparam_def.name   = name;
                subparam_def.type   = tval;
                subparam_def.count  = this->name_to_idx.at(cval);
                subparam_def.offset = this->name_to_idx.at(pval);
            }
        }
    }
}


MXEParameterSetGroupDefinition::MXEParameterSetGroupDefinition(const std::string& name, const nlohmann::ordered_json& json)
{
    size_t key_count = 0;
    size_t num_count = 0;
    for(auto it = json.begin(); it != json.end(); ++it)
    {
        num_count += isUInt(it.key());
        key_count++;
    }

    if (num_count > 0)
    {
        if (num_count != key_count)
            throw std::runtime_error("Invalid structure definition: " + std::to_string(key_count-num_count) + "/" + std::to_string(key_count) + " keys are not valid sizes.");
        
        for(auto it = json.begin(); it != json.end(); ++it)
            this->group[std::stol(it.key())] = MXEParameterSetDefinition(it.value(), name);
    }
    else
    {
        this->default_def = MXEParameterSetDefinition(json, name);
    }

}


const MXEParameterSetDefinition& MXEParameterSetGroupDefinition::getDefinition(size_t size) const
{
    if (this->hasMultipleDefinitions())
    {
        if (this->group.contains(size))
            return this->group.at(size);
        else
            throw std::runtime_error("'" + this->name + "' has no definition for size " + std::to_string(size));
    }
    else
        return this->default_def;

}


const MXEParameterSetDefinition& MXEParameterSetGroupDefinition::fullDefinition() const
{    
    if (this->hasMultipleDefinitions())
        return this->group.rbegin()->second;
    else
        return this->default_def;
}

bool  MXEParameterSetGroupDefinition::hasMultipleDefinitions() const
{
    return this->group.size() > 0;
}

std::vector<size_t> MXEParameterSetGroupDefinition::getDefinitionSizes() const
{
    std::vector<size_t> out;
    out.reserve(group.size());
    for (const auto& [k, v] : group)
        out.emplace_back(k);
    return out;
}

MXEEntityDefinition::MXEEntityDefinition(const std::string& name, const nlohmann::ordered_json& json)
{
    this->component.name = name;
    
    if (json.contains("Parameters"))
    {
        const auto& paramdefs = json.at("Parameters");
        if (paramdefs.is_array())
        {
            for (const auto& paramdef : paramdefs)
            {
                if (!paramdef.is_array()) continue;
                if (paramdef.size() != 2) continue;
                if (!paramdef.at(0).is_string()) continue;
                if (!paramdef.at(1).is_string()) continue;

                auto& param = this->component.parameters.emplace_back();
                param.member_type = paramdef.at(0);
                param.param_type  = paramdef.at(1);
            }
        }  
    }

    if (json.contains("SubEntities"))
    {
        const auto& compdefs = json.at("SubEntities");
        if (compdefs.is_array())
        {
            for (const auto& compdef : compdefs)
            {
                if (compdef.is_string())
                    this->subcomponents.push_back(compdef);
            }
        }
    }
}


inline size_t countComponentChildren(const MXEEntityDefinition& component, const EntityDefMap& defmap)
{
    size_t sz = component.subcomponents.size();
    for (const auto& c : component.subcomponents)
        sz += countComponentChildren(defmap.at(c), defmap);
    return sz;
}

void constructFlatEntityComponents(std::vector<MXEFlatEntityComponent>& out, std::vector<const MXEEntityDefinition*>& stack, const MXEEntityDefinition& entity, const EntityDefMap& defmap, int parent, int next_sibling, const std::string& parent_name="")
{
    // Check for recursive entity definitions...
    if(std::find(stack.begin(), stack.end(), &entity) != stack.end())
    {
        std::string error_msg = "ERROR: Recursive entity definition: ";
        for (const MXEEntityDefinition* const s : stack)
            error_msg += s->component.name + "->";
        error_msg += entity.component.name;
        std::cout << error_msg << std::endl;
        throw std::runtime_error(error_msg);
    }
    stack.push_back(&entity);

    int index = out.size();
    const auto& flat_entity = out.emplace_back(entity.component, parent, next_sibling, parent_name);
    for (int i=0; i < entity.subcomponents.size(); ++i)
    {   
        const auto& subc_name = entity.subcomponents[i];
        const auto& subc = defmap.at(subc_name);
        constructFlatEntityComponents(out, stack, subc, defmap, index, i != entity.subcomponents.size()-1? index + 1 + countComponentChildren(subc, defmap) : -1, flat_entity.full_name);
    }
    stack.pop_back();

}

MXEFlatEntityComponent::MXEFlatEntityComponent(const MXEEntityComponent& component, int parent, int next_sibling, const std::string& parent_name)
{
    this->parent       = parent;
    this->next_sibling = next_sibling;
    this->name         = component.name;
    this->full_name    = parent_name + "!" + component.name;
    this->parameters   = component.parameters;
}


MXEFlatEntityDefinition::MXEFlatEntityDefinition(const MXEEntityDefinition& entity, const EntityDefMap& defmap)
{
    this->components.reserve(countComponentChildren(entity, defmap));
    this->name = entity.component.name;
    std::vector<const MXEEntityDefinition*> stack;
    constructFlatEntityComponents(this->components, stack, entity, defmap, -1, -1);

}

MXEFlatEntityDefinition::MXEFlatEntityDefinition(const std::string& name, const EntityDefMap& defmap)
    : MXEFlatEntityDefinition(defmap.at(name), defmap)
{}
