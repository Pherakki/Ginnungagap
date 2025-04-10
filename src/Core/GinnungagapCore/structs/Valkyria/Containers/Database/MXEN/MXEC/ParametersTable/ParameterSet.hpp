#pragma once

#include <unordered_map>
#include <vector>

#include <exbip/exbip.hpp>
#include "Parameter.hpp"
#include "ParamDefinitions/Definition.hpp"
#include "../StringBank.hpp"
#include <GinnungagapCore/utils/StringUtils.hpp>


struct ParameterSet
{
    typedef std::vector<Parameter> Parameters;
    uint32_t ID;
    uint32_t size;
    uint64_t name_offset;
    uint64_t data_offset;

    Parameters dataset;
    std::unordered_map<std::string, std::vector<Parameters>> subparameters;
    
    std::string name;
    std::string data_name;
    std::string type_name;

    exbip::OffsetMarker data_marker;

    template<typename Parser>
    void exbip_rw(Parser& rw, MXECStringBank& sbank)
    {
        rw.rw_uint32(this->ID);
        rw.rw_uint32(this->size);
        rw.rw_memoized_string64(this->name_offset, this->name, sbank);
        rw.rw_offset64(this->data_offset);
        rw.template rw_padding<0x08>();
    }

    template<typename Parser>
    void rw_dataset(Parser& rw, const ParameterDefMap& defmap, MXECStringBank& sbank)
    {
        rw.align(rw.tell(), 0x10);
        this->data_name = stringmanip::MXEParameterSetName(this->name);
        this->type_name = stringmanip::MXEParameterSetType(this->name);
        
        if (rw.section_exists(this->data_offset, this->size))
        {
            rw.verify_and_set_stream_offset(this->data_offset, "ParametersTable::ParameterSet::Data", this->data_marker);

            if (!defmap.contains(this->type_name))
                throw std::runtime_error("Unknown Parameter Set type '" + this->type_name + "' (full parameter name: " + this->name + " at offset " + std::to_string(name_offset) + ")");
            
            const auto& mxe_def_group = defmap.at(this->type_name);
            const auto& mxe_def = mxe_def_group.getDefinition(this->size);

            size_t param_idx = 0;
            for (auto& param : rw.iterate_resizable(this->dataset, mxe_def.data.size()))
            {
                const auto& elem_def = mxe_def.data[param_idx];
                rw.rw_obj(param, elem_def.type, elem_def.type_name, sbank);
                param_idx++;
            }

            for (const auto& subdef : mxe_def.subparams)
            {
                const auto& subname = subdef.name;
                if (!defmap.contains(subdef.type))
                    throw std::runtime_error("Unknown Parameter Set type '" + this->type_name + "' used in subparameter definition '" + subname + "'");
                
                const auto& submxe_def_group = defmap.at(subdef.type);
                const auto& submxe_def = submxe_def_group.getDefinition(0);

                int64_t subcount;
                auto& delem = this->dataset[subdef.count];
                delem.extract_integer(subcount);

                if (subcount)
                {
                    uint64_t& suboffset = this->dataset[subdef.offset].data.m_uint64;

                    auto& subparams = this->subparameters[subname];

                    rw.verify_and_set_stream_offset(suboffset, "ParametersTable::ParameterSet::SubparamsData");
                    for (auto& subparam : rw.iterate_resizable(subparams, subcount))
                    {
                        size_t subparam_idx=0;
                        for (auto& param : rw.iterate_resizable(subparam, submxe_def.data.size()))
                        {
                            const auto& elem_def = submxe_def.data[subparam_idx];
                            rw.rw_obj(param, elem_def.type, elem_def.type_name, sbank);
                            subparam_idx++;
                        }
                    }
                }
            }
        }
        rw.align(rw.tell(), 0x08);
    }
};
