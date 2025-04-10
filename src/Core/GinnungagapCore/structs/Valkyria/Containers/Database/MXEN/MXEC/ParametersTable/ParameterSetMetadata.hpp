#pragma once

#include <cinttypes>
#include "ParameterSet.hpp"
#include "ParamDefinitions/Definition.hpp"


struct Vector3View
{
    bool exists=false;
    const double* _x=nullptr;
    const double* _y=nullptr;
    const double* _z=nullptr;

    const double x() const { return *this->_x; }
    const double y() const { return *this->_y; }
    const double z() const { return *this->_z; }

};


struct IntView
{
    bool exists=false;
    const int64_t* _v=nullptr;

    const int64_t v() const { return *this->_v; }
};


struct ParameterSetMetadata
{
    bool is_entity_params=false;
    bool is_paths_params=false;
    bool is_edge_params=false;
    Vector3View position;
    Vector3View rotation;
    Vector3View scale;
    IntView   team;

    ParameterSetMetadata() = default;
    ParameterSetMetadata(const ParameterSet& pset, const ParameterDefMap& defmap)
    {
        this->from_dataset(pset, defmap);
    }

    void from_dataset(const ParameterSet& pset, const ParameterDefMap& defmap)
    {
        auto& dataset  = pset.dataset;
        auto& mxe_type = pset.type_name;

        const auto& mxe_def_group = defmap.at(mxe_type);
        const auto& mxe_def = mxe_def_group.getDefinition(pset.size);
        
        if (mxe_def.position.size() == 3)
        {
            this->position.exists = true;
            this->position._x = &dataset[mxe_def.position[0]].data.m_float;
            this->position._y = &dataset[mxe_def.position[1]].data.m_float;
            this->position._z = &dataset[mxe_def.position[2]].data.m_float;
        }

        if (mxe_def.rotation.size() == 3)
        {
            this->rotation.exists = true;
            this->rotation._x = &dataset[mxe_def.rotation[0]].data.m_float;
            this->rotation._y = &dataset[mxe_def.rotation[1]].data.m_float;
            this->rotation._z = &dataset[mxe_def.rotation[2]].data.m_float;
        }

        if (mxe_def.scale.size() == 3)
        {
            this->scale.exists = true;
            this->scale._x = &dataset[mxe_def.scale[0]].data.m_float;
            this->scale._y = &dataset[mxe_def.scale[1]].data.m_float;
            this->scale._z = &dataset[mxe_def.scale[2]].data.m_float;
        }

        if (mxe_def.team != 0xFFFFFFFFFFFFFFFF)
        {
            const auto& ds = dataset[mxe_def.team];
            if (ds.is_uint())
            {
                this->team.exists = true;
                this->team._v = reinterpret_cast<const int64_t*>(&ds.data.m_uint);
            }
            else if (ds.is_int())
            {
                this->team.exists = true;
                this->team._v = &ds.data.m_int;
            }
        }
    }
};
