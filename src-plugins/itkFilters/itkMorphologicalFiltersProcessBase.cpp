/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMorphologicalFiltersProcessBase.h>

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkMorphologicalFiltersProcessBase_p.h>

itkMorphologicalFiltersProcessBase::itkMorphologicalFiltersProcessBase(itkMorphologicalFiltersProcessBase *parent) 
    : itkFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(this), parent)
{
    DTK_D(itkMorphologicalFiltersProcessBase);
    
    d->input = NULL;
    d->output = NULL;

    d->radius[0] = 0;
    d->radius[1] = 0;
    d->radius[2] = 0;

    d->radiusMm[0] = 0;
    d->radiusMm[1] = 0;
    d->radiusMm[2] = 0;

    d->isRadiusInPixels = false;    
    d->description = "";
}

itkMorphologicalFiltersProcessBase::itkMorphologicalFiltersProcessBase(const itkMorphologicalFiltersProcessBase& other) 
    : itkFiltersProcessBase(*new itkMorphologicalFiltersProcessBasePrivate(*other.d_func()), other)
{
    
}

itkMorphologicalFiltersProcessBase::~itkMorphologicalFiltersProcessBase()
{
    
}

void itkMorphologicalFiltersProcessBase::setParameter(double data, int channel)
{
    if (channel <= 1)
    {
        DTK_D(itkMorphologicalFiltersProcessBase);

        d->radius[0] = data;
        d->radius[1] = data;
        d->radius[2] = data;

        if (channel == 1) // data is in pixels
        {
            d->isRadiusInPixels = true;
        }
        if (channel == 0) //data is in mm
        {
            d->isRadiusInPixels = false;
        }
    }
}

//only called if not defined in subclasses (e.g. dilate/erodeFilter)
int itkMorphologicalFiltersProcessBase::update()
{
    DTK_D(itkMorphologicalFiltersProcessBase);

    if (d->input)
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
        else if ( id == "itkDataImageFloat3" )
        {
            return d->update<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            return d->update<double>();
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
