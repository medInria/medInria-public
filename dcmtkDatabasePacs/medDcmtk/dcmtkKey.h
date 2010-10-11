#ifndef DCMTKKEY_H
#define DCMTKKEY_H

#include <iostream>

#include "dcmtkContainer.h"

class dcmtkKey
{
public:
    int group;
    int elem;
    std::string value;
};


#endif DCMTKKEYCONTAINER_H
