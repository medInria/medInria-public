/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersInvertProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkFiltersInvertProcess_p.h>

itkFiltersInvertProcess::itkFiltersInvertProcess(itkFiltersInvertProcess *parent)
    : itkFiltersProcessBase(*new itkFiltersInvertProcessPrivate(this), parent)
{
    DTK_D(itkFiltersInvertProcess);

    d->filter = this;
    d->output = NULL;

     d->description = tr("ITK invert intensity filter");
}


itkFiltersInvertProcess::itkFiltersInvertProcess(const itkFiltersInvertProcess& other)
    : itkFiltersProcessBase(*new itkFiltersInvertProcessPrivate(*other.d_func()), other)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersInvertProcess::~itkFiltersInvertProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersInvertProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkInvertProcess", createitkFiltersInvertProcess);
}

//-------------------------------------------------------------------------------------------

int itkFiltersInvertProcess::update ( void )
{
    DTK_D(itkFiltersInvertProcess);

    if ( d->input )
    {
        QString id = d->input->identifier();

        if ( id == "itkDataImageChar3" )
        {
            return d->update<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            return d->update<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            return d->update<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            return d->update<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            return d->update<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            return d->update<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            return d->update<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            return d->update<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" || "itkDataImageDouble3" )
        {
            qDebug() << "Error: Invert image filter does not suport floating pixel values";
            return medAbstractProcess::PIXEL_TYPE;
        }
        else
        {
            qDebug() << description()
                     <<", Error : pixel type not yet implemented ("
                    << id
                    << ")";
        }
    }
    return medAbstractProcess::DIMENSION_3D;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersInvertProcess ( void )
{
    return new itkFiltersInvertProcess;
}

