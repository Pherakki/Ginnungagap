#pragma once
#include <vector>


class MXENInterfaceVC4
{
public:
    int mergefile;
    std::vector<int> texmerges;
    std::vector<int> global_parameters;
    std::vector<int> entities; // Paths contained within entity parameters
    std::vector<int> assets;
};
