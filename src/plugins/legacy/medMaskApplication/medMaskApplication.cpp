/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medMaskApplication.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <itkMaskImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>

#include <medAbstractDataFactory.h>
#include <medUtilities.h>

// /////////////////////////////////////////////////////////////////
// medMaskApplicationPrivate
// /////////////////////////////////////////////////////////////////

class medMaskApplicationPrivate
{
public:
    dtkSmartPointer <medAbstractData> mask;
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> output;
    double maskBackgroundValue;

    template <class ImagePixelType, class MaskPixelType> int update()
    {
        if (input->data() && mask->data())
        {
            typedef itk::Image<ImagePixelType, 3 > ImageType;
            typedef itk::Image<MaskPixelType,  3 > MaskType;

            typedef itk::MaskImageFilter< ImageType,  MaskType> MaskFilterType;
            typename MaskFilterType::Pointer maskFilter = MaskFilterType::New();

            typename ImageType::Pointer imgInput = dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() )) ;
            typename MaskType::Pointer maskInput = dynamic_cast<MaskType *>  ( ( itk::Object* ) ( mask->data() )) ;

            try
            {
                maskInput->SetOrigin(imgInput->GetOrigin());
                maskInput->SetSpacing(imgInput->GetSpacing());
                maskFilter->SetInput(imgInput);
                maskFilter->SetMaskingValue(maskBackgroundValue);
                maskFilter->SetMaskImage(maskInput);

                //Outside values set to the lowest reachable value
                typedef itk::MinimumMaximumImageCalculator <ImageType> ImageCalculatorFilterType;

                typename ImageCalculatorFilterType::Pointer imageCalculatorFilter
                        = ImageCalculatorFilterType::New ();
                imageCalculatorFilter->SetImage(imgInput);
                imageCalculatorFilter->ComputeMinimum();
                maskFilter->SetOutsideValue(std::min(double(imageCalculatorFilter->GetMinimum()), 0.0));
                maskFilter->Update();

                QString identifier = input->identifier();
                output = medAbstractDataFactory::instance()->createSmartPointer ( identifier );
                output->setData(maskFilter->GetOutput());
            }
            catch( itk::ExceptionObject & err )
            {
                std::cerr << "ExceptionObject caught in medMaskApplication!" << std::endl;
                std::cerr << err << std::endl;
                return medAbstractProcessLegacy::FAILURE;
            }

            medUtilities::setDerivedMetaData(output, input, "masked");

            return medAbstractProcessLegacy::SUCCESS;
        }
        return medAbstractProcessLegacy::FAILURE;
    }
};

// /////////////////////////////////////////////////////////////////
// medMaskApplication
// /////////////////////////////////////////////////////////////////

medMaskApplication::medMaskApplication() : medAbstractProcessLegacy(), d(new medMaskApplicationPrivate)
{
    d->input  = nullptr;
    d->mask   = nullptr;
    d->output = nullptr;
    d->maskBackgroundValue = 0.0;
}

medMaskApplication::~medMaskApplication()
{
    delete d;
    d = nullptr;
}

bool medMaskApplication::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medMaskApplication", createMedMaskApplication);
}

QString medMaskApplication::description() const
{
    return "medMaskApplication";
}

void medMaskApplication::setInput(medAbstractData *data, int channel)
{
    if (channel == 0)
    {
        d->mask = data;
    }

    if (channel == 1)
    {
        d->input = data;
    }
}

void medMaskApplication::setParameter(double data)
{
    d->maskBackgroundValue = data;
}

void medMaskApplication::clearInput(int channel)
{
    if (channel == 0)
    {
        d->mask = nullptr;
    }

    if (channel == 1)
    {
        d->output = nullptr;
        d->input = nullptr;
    }
}

int medMaskApplication::update()
{
    int res = medAbstractProcessLegacy::FAILURE;

    if (d->input)
    {
        QString id = d->input->identifier();

        if ( id == "itkDataImageChar3" )
        {
            res = updateMaskType<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            res = updateMaskType<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            res = updateMaskType<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            res = updateMaskType<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            res = updateMaskType<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            res = updateMaskType<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            res = updateMaskType<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            res = updateMaskType<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            res = updateMaskType<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            res = updateMaskType<double>();
        }
        else
        {
            res = medAbstractProcessLegacy::PIXEL_TYPE;
        }
    }
    return res;
}

template <typename IMAGE>
int medMaskApplication::updateMaskType()
{
    int res = medAbstractProcessLegacy::FAILURE;

    if (d->mask)
    {
        QString id = d->mask->identifier();

        if ( id == "itkDataImageChar3" )
        {
            res = d->update<IMAGE, char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            res = d->update<IMAGE, unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            res = d->update<IMAGE, short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            res = d->update<IMAGE, unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            res = d->update<IMAGE, int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            res = d->update<IMAGE, unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            res = d->update<IMAGE, long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            res = d->update<IMAGE, unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            res = d->update<IMAGE, float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            res = d->update<IMAGE, double>();
        }
        else
        {
            res = medAbstractProcessLegacy::PIXEL_TYPE;
        }
    }
    return res;
}


medAbstractData * medMaskApplication::output()
{
    return d->output;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createMedMaskApplication()
{
    return new medMaskApplication;
}
