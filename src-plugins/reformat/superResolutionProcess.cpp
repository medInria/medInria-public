/*=========================================================================

 Plugin medInria - Super Resolution

 Copyright (c) IHU LIRYC 2013. All rights reserved.

 Author : Mathilde Merle -- mathilde.merle @ ihu-liryc.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "superResolutionProcess.h"
#include "shapeBasedInterpolation.h"

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <itkCastImageFilter.h>
#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>
#include <medUtilities.h>

// /////////////////////////////////////////////////////////////////
// superResolutionProcessPrivate
// /////////////////////////////////////////////////////////////////

class superResolutionProcessPrivate
{
public:
    dtkSmartPointer<medAbstractData> input0;
    dtkSmartPointer<medAbstractData> input1;
    dtkSmartPointer<medAbstractData> input2;
    dtkSmartPointer<medAbstractData> input3;

    dtkSmartPointer<medAbstractData> output;

    shapeBasedInterpolation shapeBasedInterpolationFilter;
};


// /////////////////////////////////////////////////////////////////
// superResolutionProcess
// /////////////////////////////////////////////////////////////////
superResolutionProcess::superResolutionProcess(void) : medAbstractProcess(), d(new superResolutionProcessPrivate())
{
    d->input0 = NULL;
    d->input1 = NULL;
    d->input2 = NULL;
    d->input3 = NULL;

    d->output = NULL;
}

superResolutionProcess::~superResolutionProcess(void)
{
    delete d;
    d = NULL;
}

bool superResolutionProcess::registered(void)
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("superResolutionProcess", createsuperResolutionProcess);
}

QString superResolutionProcess::description(void) const
{
    return "superResolutionProcess";
}

// Input data to the plugin is set through here
void superResolutionProcess::setInput( medAbstractData *data, int channel )
{
    // Fill variables with input data
    switch (channel)
    {
        case 0:
            d->input0 = data;
        case 1:
            d->input1 = data;
        case 2:
            d->input2 = data;
        case 3:
            d->input3 = data;
    }
}

// Method to actually start the filter
int superResolutionProcess::update ( void )
{
    if ( d->input0 && d->input1 && d->input2 && d->input3 )
    {
        // cast to unsigned char and set input in shapeBasedInterpolationFilter
        bool res0 = castToUChar3(d->input0);
        bool res1 = castToUChar3(d->input1);
        bool res2 = castToUChar3(d->input2);
        bool res3 = castToUChar3(d->input3);

        if ( res0 == DTK_SUCCEED && res1 == DTK_SUCCEED && res2 == DTK_SUCCEED && res3 == DTK_SUCCEED )
        {
            connect(&d->shapeBasedInterpolationFilter , SIGNAL(progressed(int)), this, SIGNAL(progressed(int)));
            d->shapeBasedInterpolationFilter.run();

            d->output = medAbstractDataFactory::instance()->create ("itkDataImageUChar3");
            d->output->setData(d->shapeBasedInterpolationFilter.output()); // update output data
            medUtilities::setDerivedMetaData(d->output, d->input0, "super resolution");

            return DTK_SUCCEED;
        }
    }
    return DTK_FAILURE;
}

bool superResolutionProcess::castToUChar3(dtkSmartPointer<medAbstractData> image)
{
    QString id = image->identifier();

    if ( id == "itkDataImageChar3" )
    {
        return cast< itk::Image <char,3> >(image);
    }
    else if ( id == "itkDataImageUChar3" )
    {
        return cast< itk::Image <unsigned char,3> >(image);
    }
    else if ( id == "itkDataImageShort3" )
    {
        return cast< itk::Image <short,3> >(image);
    }
    else if ( id == "itkDataImageUShort3" )
    {
        return cast< itk::Image <unsigned short,3> >(image);
    }
    else if ( id == "itkDataImageInt3" )
    {
        return cast< itk::Image <int,3> >(image);
    }
    else if ( id == "itkDataImageUInt3" )
    {
        return cast< itk::Image <unsigned int,3> >(image);
    }
    else if ( id == "itkDataImageLong3" )
    {
        return cast< itk::Image <long,3> >(image);
    }
    else if ( id== "itkDataImageULong3" )
    {
        return cast< itk::Image <unsigned long,3> >(image);
    }
    else if ( id == "itkDataImageFloat3" )
    {
        return cast< itk::Image <float,3> >(image);
    }
    else if ( id == "itkDataImageDouble3" )
    {
        return cast< itk::Image <double,3> >(image);
    }
    else
    {
        qDebug() << "Error : pixel type not yet implemented ("
        << id
        << ")";
    }
    return DTK_FAILURE;
}

template <typename IMAGE>
bool
superResolutionProcess::cast(dtkSmartPointer<medAbstractData> image)
{
    IMAGE* tmpPtr = dynamic_cast<IMAGE*> ((itk::Object*)(image->data()));

    if (tmpPtr)
    {
        typedef itk::CastImageFilter <IMAGE, MaskType> CastFilter;
        typename CastFilter::Pointer castFilter = CastFilter::New();

        castFilter->SetInput(dynamic_cast<IMAGE*>((itk::Object*)(tmpPtr)));
        castFilter->Update();

        d->shapeBasedInterpolationFilter.setInput(castFilter->GetOutput());

        return DTK_SUCCEED;
    }
    return DTK_FAILURE;
}

// The output will be available through here
medAbstractData * superResolutionProcess::output ( void )
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createsuperResolutionProcess(void)
{
    return new superResolutionProcess;
}
