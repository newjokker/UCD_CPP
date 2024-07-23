

#include <iostream>
#include <vector>
#include "include/printCpp.hpp"




void print_vector(std::vector<std::string> assign_vector)
{
    for(int i=0; i<assign_vector.size(); i++)
    {
        std::cout << assign_vector[i] << std::endl;
    }
}

