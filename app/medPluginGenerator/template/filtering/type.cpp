/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "%1.h"

#include <dtkCore/dtkAbstract%2Factory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstract%2.h>

// /////////////////////////////////////////////////////////////////
// %1Private
// /////////////////////////////////////////////////////////////////

class %1Private
{
public:
    dtkSmartPointer <dtkAbstractData> input;
    dtkSmartPointer <dtkAbstractData> output;
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

void %1::setInput ( dtkAbstractData *data )
{
    if ( !data )
        return;
    
    QString identifier = data->identifier();
    
    d->output = dtkAbstractDataFactory::instance()->createSmartPointer ( identifier );
    
    d->input = data;
}    

void %1::setParameter ( double  data, int channel )
{
    // Here comes a switch over channel to handle parameters
}

int %1::update()
{
    if ( !d->input )
        return -1;
    
    // Your update code comes in here
    
    return EXIT_SUCCESS;
}        

dtkAbstractData * %1::output()
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstract%2 *create%3()
{
    return new %1;
}
