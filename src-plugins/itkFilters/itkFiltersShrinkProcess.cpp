/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersShrinkProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkFiltersShrinkProcess_p.h>

//-------------------------------------------------------------------------------------------

itkFiltersShrinkProcess::itkFiltersShrinkProcess(itkFiltersShrinkProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersShrinkProcessPrivate(this), parent)
{
    DTK_D(itkFiltersShrinkProcess);
    
    d->filter = this;
    d->input = NULL;
    d->output = NULL;
    
    d->description = tr("ITK shrink filter");
}


itkFiltersShrinkProcess::itkFiltersShrinkProcess(const itkFiltersShrinkProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersShrinkProcessPrivate(*other.d_func()), other)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersShrinkProcess::~itkFiltersShrinkProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersShrinkProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkShrinkProcess", createitkFiltersShrinkProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersShrinkProcess::setParameter(double data, int channel)
{   
    DTK_D(itkFiltersShrinkProcess);
    
    if (channel > 2)
        return;
    
    switch ( channel )
    {
    case 0:
        d->shrinkFactors[0] = ( unsigned int ) data;
        break;
    case 1:
        d->shrinkFactors[1] = ( unsigned int ) data;
        break;
    case 2:
        d->shrinkFactors[2] = ( unsigned int ) data;
        break;
    }
}

//-------------------------------------------------------------------------------------------

int itkFiltersShrinkProcess::update ( void )
{
    DTK_D(itkFiltersShrinkProcess);
    
    if ( !d->input )
        return -1;

    QString id = d->input->identifier();

    if ( id == "itkDataImageChar3" )
    {
        d->update<char>();
    }
    else if ( id == "itkDataImageUChar3" )
    {
        d->update<unsigned char>();
    }
    else if ( id == "itkDataImageShort3" )
    {
        d->update<short>();
    }
    else if ( id == "itkDataImageUShort3" )
    {
        d->update<unsigned short>();
    }
    else if ( id == "itkDataImageInt3" )
    {
        d->update<int>();
    }
    else if ( id == "itkDataImageUInt3" )
    {
        d->update<unsigned int>();
    }
    else if ( id == "itkDataImageLong3" )
    {
        d->update<long>();
    }
    else if ( id== "itkDataImageULong3" )
    {
        d->update<unsigned long>();
    }
    else if ( id == "itkDataImageFloat3" )
    {
        d->update<float>();
    }
    else if ( id == "itkDataImageDouble3" )
    {
        d->update<double>();
    }
    else
    {
        qDebug() << "itkFiltersShrinkProcess Error : pixel type not yet implemented ("
        << id
        << ")";
        return -1;
    }

    return EXIT_SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersShrinkProcess ( void )
{
    return new itkFiltersShrinkProcess;
}
