#include "dcmtkKeyContainer.h"


//---------------------------------------------------------------------------------------------

const char* dcmtkKeyContainer::findKeyValue( int group, int elem )
{
    const char* result = "";
    for(iter = data.begin(); iter != data.end(); iter++)
    {
        if ( ((*iter)->group == group) && ((*iter)->elem == elem) )
        {
                result = (*iter)->value.c_str();
                break;
        }
    }
    return result;
}

//---------------------------------------------------------------------------------------------
