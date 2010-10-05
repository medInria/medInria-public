#ifndef DCMTKKEYCONTAINER_H
#define DCMTKKEYCONTAINER_H

#include <iostream>

#include "dcmtkContainer.h"

struct dcmtkKey
{
public:
    int group;
    int elem;
    std::string value;
};

class dcmtkKeyContainer : public dcmtkContainer<dcmtkKey*>
{
public:
       const char* findKeyValue(int group, int elem);
};

#endif DCMTKKEYCONTAINER_H