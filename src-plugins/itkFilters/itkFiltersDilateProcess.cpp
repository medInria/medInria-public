/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersDilateProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkFiltersDilateProcess_p.h>

//-------------------------------------------------------------------------------------------

itkFiltersDilateProcess::itkFiltersDilateProcess(itkFiltersDilateProcess *parent) 
    : itkFiltersProcessBase(*new itkFiltersDilateProcessPrivate(this), parent)
{
    DTK_D(itkFiltersDilateProcess);
    
     d->filter = this;
    d->output = NULL;
    d->radius[0] = 0;
    d->radius[1] = 0;
    d->radius[2] = 0;

    d->radiusMm[0] = 0;
    d->radiusMm[1] = 0;
    d->radiusMm[2] = 0;

    d->isRadiusInPixels = false;
    d->radiusInPixels = 0;
    d->description = tr("ITK Dilate filter");
}


itkFiltersDilateProcess::itkFiltersDilateProcess(const itkFiltersDilateProcess& other) 
    : itkFiltersProcessBase(*new itkFiltersDilateProcessPrivate(*other.d_func()), other)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersDilateProcess::~itkFiltersDilateProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersDilateProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkDilateProcess", createitkFiltersDilateProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersDilateProcess::setParameter(double data, int channel)
{
    if (channel > 1)
        return;
    DTK_D(itkFiltersDilateProcess);
    d->radiusInPixels = data;

    d->radius[0] = data;
    d->radius[1] = data;
    d->radius[2] = data;

    if (channel == 1) // data is in pixels
        d->isRadiusInPixels = true;

    if (channel == 0) //data is in mm
        d->isRadiusInPixels = false;
}

//-------------------------------------------------------------------------------------------

int itkFiltersDilateProcess::update ( void )
{
    DTK_D(itkFiltersDilateProcess);
    
    if ( !d->input )
        return -1;

    QString id = d->input->identifier();

    qDebug() << "itkFiltersDilateProcess, update : " << id;

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
        qDebug() << "Error : pixel type not yet implemented ("
        << id
        << ")";
        return -1;
    }

    return EXIT_SUCCESS;
}


// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersDilateProcess ( void )
{
    return new itkFiltersDilateProcess;
}
