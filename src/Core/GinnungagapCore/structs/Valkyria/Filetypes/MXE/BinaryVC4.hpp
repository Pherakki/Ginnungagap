#pragma once

#include <GinnungagapCore/structs/Valkyria/Containers/Database/MXEN/BinaryVC4.hpp>



struct MXE
{
    MXEN mxen;
    EOFC eofc;
public:
    template<typename Parser>
    void exbip_rw(Parser& rw, const ParameterDefMap& params_defmap)
    {
        rw.rw_obj(mxen, params_defmap);
        rw.rw_obj(eofc);
    }
};

