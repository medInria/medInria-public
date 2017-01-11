/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkN4BiasCorrection.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractProcess.h>
#include <medAbstractDataFactory.h>


#include "itkN4BiasCorrection_p.h"

// /////////////////////////////////////////////////////////////////
// itkN4BiasCorrection
// /////////////////////////////////////////////////////////////////

itkN4BiasCorrection::itkN4BiasCorrection() : medAbstractProcess(), d(new itkN4BiasCorrectionPrivate)
{
    d->output = NULL;
    d->saveBias = false;
}

itkN4BiasCorrection::~itkN4BiasCorrection()
{
    delete d;
    d = NULL;
}

bool itkN4BiasCorrection::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkN4BiasCorrection", createItkN4BiasCorrection);
}

QString itkN4BiasCorrection::description() const
{
    return "itkN4BiasCorrection";
}

void itkN4BiasCorrection::setInput ( medAbstractData *data, int channel )
{
    if (!channel)
    {
        d->input = data;
    }
    else 
    {
        d->mask = data;
    }
}    

void itkN4BiasCorrection::setParameter ( double  data, int channel )
{
    if (channel == 0)
        d->splineDistance = data;
    if (channel == 1)
        d->bfFWHM = (float)data;
    if (channel == 2)
        d->convergenceThreshold = data;
    if (channel == 3)
        d->bsplineOrder = data;
    if (channel == 4)
        d->shrinkFactor = data;
    if (channel == 5)
        d->nbHistogramBins = data;
    if (channel == 6)
        d->wienerFilterNoise = data;
    if (channel == 7)
        d->saveBias = data;
    if (channel>=10)
        d->numberOfIterations.push_back(data);
}

int itkN4BiasCorrection::update()
{
    int res = DTK_FAILURE;

    if (d->input)
    {
        QString id = d->input->identifier();

        if ( id == "itkDataImageChar3" )
        {
            res = d->update<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            res = d->update<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            res = d->update<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            res = d->update<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            res = d->update<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            res = d->update<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            res = d->update<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            res = d->update<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            res = d->update<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            res = d->update<double>();
        }
        else
        {
            res = medAbstractProcess::PIXEL_TYPE;
        }
    }

    return res;
}        

medAbstractData * itkN4BiasCorrection::output()
{
    return d->output;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createItkN4BiasCorrection()
{
    return new itkN4BiasCorrection;
}
