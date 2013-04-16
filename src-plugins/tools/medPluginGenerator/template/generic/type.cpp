/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/




#include "%1.h"

#include <dtkCore/dtkAbstract%2Factory.h>

// /////////////////////////////////////////////////////////////////
// %1Private
// /////////////////////////////////////////////////////////////////

class %1Private
{
public:
};

// /////////////////////////////////////////////////////////////////
// %1
// /////////////////////////////////////////////////////////////////

%1::%1() : dtkAbstract%2(), d(new %1Private)
{
    
}

%1::~%1()
{
    
}

bool %1::registered()
{
    return dtkAbstract%2Factory::instance()->register%2Type("%1", create%3);
}

QString %1::description() const
{
    return "%1";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstract%2 *create%3()
{
    return new %1;
}
