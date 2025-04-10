#pragma once

#include "MXEC/BinaryVC4.hpp"
#include <GinnungagapCore/structs/Valkyria/Containers/Metadata/EOFC/BinaryVC4.hpp>


struct MXEN : public BaseContainerVC4<MXEN>
{
    MXEC mxec;
    EOFC eofc;
public:
    template<typename Parser>
    void valk_rw(Parser& rw, const ParameterDefMap& params_defmap)
    {
    }

    template<typename Parser>
    void rw_subcontainers(Parser& rw, const ParameterDefMap& params_defmap)
    {
        rw.rw_obj(mxec, params_defmap);
        rw.rw_obj(eofc);
    }
};
