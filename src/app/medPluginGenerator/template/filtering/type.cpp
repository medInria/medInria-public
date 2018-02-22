/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <%1.h>

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>


// /////////////////////////////////////////////////////////////////
// %1Private
// /////////////////////////////////////////////////////////////////

class %1Private
{
public:
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> output;
};

// /////////////////////////////////////////////////////////////////
// %1
// /////////////////////////////////////////////////////////////////

%1::%1() : dtkAbstractProcess(), d(new %1Private)
{
    
}

%1::~%1()
{
    
}

bool %1::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("%1", create);
}

QString %1::description() const
{
    return "%1";
}

void %1::setInput ( medAbstractData *data )
{
    if ( !data )
        return;
    
    QString identifier = data->identifier();
    
    d->output = medAbstractDataFactory::instance()->createSmartPointer ( identifier );
    
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

medAbstractData * %1::output()
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *create()
{
    return new %1;
}
