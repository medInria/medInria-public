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

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcess.h>

#include <medMetaDataKeys.h>

#include <itkCannyEdgeDetectionImageFilter.h>
#include <itkCastImageFilter.h>

// /////////////////////////////////////////////////////////////////
// superResolutionProcessPrivate
// /////////////////////////////////////////////////////////////////

class superResolutionProcessPrivate
{
public:
    superResolutionProcess *parent;
    superResolutionProcessPrivate() :parent(NULL){}

    medAbstractData * input0;
    medAbstractData * input1;
    medAbstractData * input2;
    medAbstractData * input3;
    medAbstractData * output;

    // casted in unsigned char
    medAbstractData * d0;
    medAbstractData * d1;
    medAbstractData * d2;
    medAbstractData * d3;
};


// /////////////////////////////////////////////////////////////////
// superResolutionProcess
// /////////////////////////////////////////////////////////////////
superResolutionProcess::superResolutionProcess(void) : medAbstractProcess(), d(new superResolutionProcessPrivate())
{
    d->parent = this;
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
    if (!data)
        return;

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
    if ( !d->input0 || !d->input1  || !d->input2  || !d->input3 )
    {
        qDebug() << "in update method : d->input is NULL";
        return -1;
    }

    d->d0 = castToUChar3(d->input0);
    d->d1 = castToUChar3(d->input1);
    d->d2 = castToUChar3(d->input2);
    d->d3 = castToUChar3(d->input3);

    if ( !d->d0 || !d->d1  || !d->d2  || !d->d3 )
        return -1;

    runButton<itk::Image<unsigned char, 3> >();

    return EXIT_SUCCESS;
}

template <class ImageType> void superResolutionProcess::runButton()
{
    // compute process
    typedef itk::Image<unsigned char, 3> MaskType;
    typedef itk::CastImageFilter <ImageType, MaskType> CastFilter0;
    typedef itk::CastImageFilter <ImageType, MaskType> CastFilter1;
    typedef itk::CastImageFilter <ImageType, MaskType> CastFilter2;
    typedef itk::CastImageFilter <ImageType, MaskType> CastFilter3;

    typename CastFilter0::Pointer castFilter0 = CastFilter0::New();
    castFilter0->SetInput(0,dynamic_cast<ImageType*>((itk::Object*)(d->d0->data())));
    castFilter0->Update();

    typename CastFilter1::Pointer castFilter1 = CastFilter1::New();
    castFilter1->SetInput(0,dynamic_cast<ImageType*>((itk::Object*)(d->d1->data())));
    castFilter1->Update();

    typename CastFilter2::Pointer castFilter2 = CastFilter2::New();
    castFilter2->SetInput(0,dynamic_cast<ImageType*>((itk::Object*)(d->d2->data())));
    castFilter2->Update();

    typename CastFilter3::Pointer castFilter3 = CastFilter3::New();
    castFilter3->SetInput(0,dynamic_cast<ImageType*>((itk::Object*)(d->d3->data())));
    castFilter3->Update();

    d->output = medAbstractDataFactory::instance()->create ("itkDataImageUChar3");

    shapeBasedInterpolation shapeBasedInterpolationFilter;

    shapeBasedInterpolationFilter.setInput(castFilter0->GetOutput(), castFilter1->GetOutput(), castFilter2->GetOutput(), castFilter3->GetOutput());
    connect(&shapeBasedInterpolationFilter , SIGNAL(progressed(int)), this, SIGNAL(progressed(int)));
    shapeBasedInterpolationFilter.run();
    d->output->setData(shapeBasedInterpolationFilter.output()); // update output data
}

medAbstractData* superResolutionProcess::castToUChar3(medAbstractData* image)
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
        return NULL;
    }
}

template <typename IMAGE>
medAbstractData*
superResolutionProcess::cast(medAbstractData* image)
{
    IMAGE* tmpPtr = dynamic_cast<IMAGE*> ((itk::Object*)(image->data()));

    if (!tmpPtr)
        return 0;

    //typedef itk::Image<unsigned char, 3> NewImageType;
    typedef itk::CastImageFilter <IMAGE, MaskType> CastFilter;
    typename CastFilter::Pointer castFilter = CastFilter::New();

    castFilter->SetInput(dynamic_cast<IMAGE*>((itk::Object*)(tmpPtr)));
    castFilter->Update();
    medAbstractData * output = medAbstractDataFactory::instance()->create("itkDataImageUChar3");
    output->setData(castFilter->GetOutput());
    return output;
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
