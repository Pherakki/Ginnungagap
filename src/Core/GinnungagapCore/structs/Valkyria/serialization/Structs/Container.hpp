#pragma once

#include "ContainerHeader.hpp"
#include <exbip/exbip.hpp>

template<typename Implementation>
class BaseContainerVC4 : public exbip::CRTPBase<Implementation>
{
public:
    ContainerHeaderVC4 header;

    template<typename Parser, typename... Args>
    void exbip_rw(Parser& rw, Args... args);

    template<typename Parser>
    void rw_header(Parser& rw);

    template<typename Parser>
    void rw_subcontainers(Parser& rw);
};

template<typename Implementation>
template<typename Parser, typename... Args>
void BaseContainerVC4<Implementation>::exbip_rw(Parser& rw, Args... args)
{
    auto ho = rw.newOrigin();
    this->downcast()->rw_header(rw);
    auto o = rw.newOrigin();
    this->downcast()->valk_rw(rw, std::forward<Args>(args)...);
    this->downcast()->header.check_body_size(rw);
    this->downcast()->rw_subcontainers(rw, std::forward<Args>(args)...);
    this->downcast()->header.check_size(rw);
}

template<typename Implementation>
template<typename Parser>
void BaseContainerVC4<Implementation>::rw_header(Parser& rw)
{
    rw.rw_obj(this->header);
    this->header.check_header_size(rw);
}


// class UnknownContainer : BaseContainer
// {
// public:
//     //std::vector<uint8_t> payload;
// };

