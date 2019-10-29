/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractProcess.h>
#include <medAbstractDataFactory.h>
#include <medN4BiasCorrection.h>
#include <medN4BiasCorrection_p.h>
#include <medUtilitiesITK.h>

// /////////////////////////////////////////////////////////////////
// medN4BiasCorrection
// /////////////////////////////////////////////////////////////////

medN4BiasCorrection::medN4BiasCorrection()
    : medAbstractProcessLegacy(), d(new medN4BiasCorrectionPrivate)
{
    d->output = nullptr;
    d->saveBias = false;
}

medN4BiasCorrection::~medN4BiasCorrection()
{
    delete d;
    d = nullptr;
}

bool medN4BiasCorrection::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medN4BiasCorrection", createMedN4BiasCorrection);
}

QString medN4BiasCorrection::description() const
{
    return "medN4BiasCorrection";
}

void medN4BiasCorrection::setInput(medAbstractData *data, int channel)
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

void medN4BiasCorrection::setParameter(double data, int channel)
{
    switch (channel)
    {
        case 0:
            d->splineDistance = data;
            break;
        case 1:
            d->bfFWHM = static_cast<float>(data);
            break;
        case 2:
            d->convergenceThreshold = data;
            break;
        case 3:
            d->bsplineOrder = data;
            break;
        case 4:
            d->shrinkFactor = data;
            break;
        case 5:
            d->nbHistogramBins = data;
            break;
        case 6:
            d->wienerFilterNoise = data;
            break;
        case 7:
            d->saveBias = data;
            break;
        default:
            if (channel >= 10)
            {
                d->numberOfIterations.push_back(data);
            }
            break;
    }
}

int medN4BiasCorrection::update()
{
    int res = medAbstractProcessLegacy::FAILURE;

    if (d->input)
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&medN4BiasCorrectionPrivate::update, d, d->input);
    }

    return res;
}        

medAbstractData * medN4BiasCorrection::output()
{
    return d->output;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createMedN4BiasCorrection()
{
    return new medN4BiasCorrection;
}
