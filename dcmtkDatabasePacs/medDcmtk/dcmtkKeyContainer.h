#ifndef DCMTKKEYCONTAINER_H
#define DCMTKKEYCONTAINER_H

#include <iostream>

#include "dcmtkContainer.h"

class dcmtkKey
{
public:
    int group;
    int elem;
    std::string value;

    inline bool operator==(const dcmtkKey key) const {
        if(group==key.group){
                 if(elem==key.elem)
                  return 1;
        }
        else 
            return 0;

    }
};

class dcmtkKeyContainer : public dcmtkContainer<dcmtkKey*>
{

};

#endif DCMTKKEYCONTAINER_H