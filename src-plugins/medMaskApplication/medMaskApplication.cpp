/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medMaskApplication.h"

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractImageData.h>
#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medUtilities.h>

#include <itkExtractImageFilter.h>
#include <itkMaskImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>

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
    int frame;

    template <class ImagePixelType, class MaskPixelType> int update()
    {
        if (input->data() && mask->data())
        {
            // Extract 3d from 4d volumes thanks to the frame number
            if (input->identifier().contains("4"))
            {
                extract4dto3d<ImagePixelType>(0, input->identifier());
            }
            if (mask->identifier().contains("4"))
            {
                extract4dto3d<MaskPixelType>(1, mask->identifier());
            }

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
                output->setData(maskFilter->GetOutput());
            }
            catch( itk::ExceptionObject & err )
            {
                std::cerr << "ExceptionObject caught in medMaskApplication!" << std::endl;
                std::cerr << err << std::endl;
                return DTK_FAILURE;
            }

            medUtilities::setDerivedMetaData(output, input, "masked");

            return DTK_SUCCEED;
        }
        return DTK_FAILURE;
    }

    template <class MaskPixelType> void extract4dto3d(int dataNumber, QString identifier)
    {
        typedef itk::Image <MaskPixelType, 3> Mask3dType;
        typedef itk::Image <MaskPixelType, 4> Mask4dType;

        typename Mask4dType::Pointer mask4dInput;
        if (!dataNumber)
            mask4dInput = dynamic_cast<Mask4dType *>  ( ( itk::Object* ) ( input->data() )) ;
        else
            mask4dInput = dynamic_cast<Mask4dType *>  ( ( itk::Object* ) ( mask->data() )) ;

        // 3d start
        typename Mask4dType::IndexType desiredStart;
        desiredStart.Fill(0);
        desiredStart[3] = frame;

        // 3d size
        typename Mask4dType::SizeType desiredSize = mask4dInput->GetLargestPossibleRegion().GetSize();
        desiredSize[3] = 0;

        typename Mask4dType::RegionType desiredRegion(desiredStart, desiredSize);

        typedef itk::ExtractImageFilter < Mask4dType, Mask3dType > MaskExtract3DType;
        typename MaskExtract3DType::Pointer extractMask = MaskExtract3DType::New();
        extractMask->SetExtractionRegion(desiredRegion);
        extractMask->SetInput(mask4dInput);
        extractMask->SetDirectionCollapseToIdentity();
        extractMask->Update();

        // Create a new 3d identifier
        QString newId = identifier;
        newId.remove("4");
        newId.append("3");

        // Needed to keep metadata
        dtkSmartPointer <medAbstractData> extractOutput = medAbstractDataFactory::instance()->createSmartPointer ( newId );
        extractOutput->setData(extractMask->GetOutput());

        if (!dataNumber)
        {
            medUtilities::setDerivedMetaData(extractOutput, input, "");
            input = extractOutput;
            medUtilities::setDerivedMetaData(input, extractOutput, "");
        }
        else
        {
            medUtilities::setDerivedMetaData(extractOutput, mask, "");
            mask = extractOutput;
            medUtilities::setDerivedMetaData(mask, extractOutput, "");
        }
    }
};

// /////////////////////////////////////////////////////////////////
// medMaskApplication
// /////////////////////////////////////////////////////////////////

medMaskApplication::medMaskApplication() : medAbstractProcess(), d(new medMaskApplicationPrivate)
{
    d->input  = NULL;
    d->mask   = NULL;
    d->output = NULL;
    d->maskBackgroundValue = 0;
    d->frame = 0;

}

medMaskApplication::~medMaskApplication()
{
    d->input  = NULL;
    d->mask   = NULL;
    d->output = NULL;
    delete d;
    d = NULL;
}

bool medMaskApplication::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medMaskApplication", createMedMaskApplication);
}

QString medMaskApplication::description() const
{
    return "medMaskApplication";
}

void medMaskApplication::setInput ( medAbstractData *data, int channel)
{
    if ( channel == 0)
    {
        d->mask = data;
    }

    if ( channel == 1 )
    {
        QString identifier = data->identifier();
        if (!identifier.isEmpty())
        {
            if (identifier.contains("4"))
            {
                identifier.remove("4");
                identifier.append("3");
            }
            d->output = medAbstractDataFactory::instance()->createSmartPointer ( identifier );
        }
        else
        {
            d->output = medAbstractDataFactory::instance()->createSmartPointer ( "itkDataImageChar3" );
        }
        d->input = data;
    }
}

void medMaskApplication::setParameter ( double data, int channel)
{
    if(channel == 0)
    {
        d->maskBackgroundValue = data;
    }
    else if (channel == 1)
    {
        d->frame = (int)data;
    }
}

void medMaskApplication::clearInput (int channel)
{
    if ( channel == 0)
    {
        d->mask = NULL;
    }

    if ( channel == 1 )
    {
        d->output = NULL;
        d->input = NULL;
    }
}

int medMaskApplication::update()
{
    int res = DTK_FAILURE;

    if ( d->input )
    {
        QString id = d->input->identifier();

        if ( id.contains("itkDataImageChar") )
        {
            res = updateMaskType<char>();
        }
        else if ( id.contains("itkDataImageUChar") )
        {
            res = updateMaskType<unsigned char>();
        }
        else if ( id.contains("itkDataImageShort") )
        {
            res = updateMaskType<short>();
        }
        else if ( id.contains("itkDataImageUShort") )
        {
            res = updateMaskType<unsigned short>();
        }
        else if ( id.contains("itkDataImageInt") )
        {
            res = updateMaskType<int>();
        }
        else if ( id.contains("itkDataImageUInt") )
        {
            res = updateMaskType<unsigned int>();
        }
        else if ( id.contains("itkDataImageLong") )
        {
            res = updateMaskType<long>();
        }
        else if ( id.contains("itkDataImageULong") )
        {
            res = updateMaskType<unsigned long>();
        }
        else if ( id.contains("itkDataImageFloat") )
        {
            res = updateMaskType<float>();
        }
        else if ( id.contains("itkDataImageDouble") )
        {
            res = updateMaskType<double>();
        }
        else
        {
            qDebug() << description()
                     <<", Error : pixel type not yet implemented ("
                    << id
                    << ")";
        }
    }
    return res;
}

template <typename IMAGE>
int medMaskApplication::updateMaskType()
{
    int res = DTK_FAILURE;

    if ( d->mask )
    {
        QString id = d->mask->identifier();

        if ( id.contains("itkDataImageChar") )
        {
            res = d->update<IMAGE, char>();
        }
        else if ( id.contains("itkDataImageUChar") )
        {
            res = d->update<IMAGE, unsigned char>();
        }
        else if ( id.contains("itkDataImageShort") )
        {
            res = d->update<IMAGE, short>();
        }
        else if ( id.contains("itkDataImageUShort") )
        {
            res = d->update<IMAGE, unsigned short>();
        }
        else if ( id.contains("itkDataImageInt") )
        {
            res = d->update<IMAGE, int>();
        }
        else if ( id.contains("itkDataImageUInt") )
        {
            res = d->update<IMAGE, unsigned int>();
        }
        else if ( id.contains("itkDataImageLong") )
        {
            res = d->update<IMAGE, long>();
        }
        else if ( id.contains("itkDataImageULong") )
        {
            res = d->update<IMAGE, unsigned long>();
        }
        else if ( id.contains("itkDataImageFloat") )
        {
            res = d->update<IMAGE, float>();
        }
        else if ( id.contains("itkDataImageDouble") )
        {
            res = d->update<IMAGE, double>();
        }
        else
        {
            qDebug() << description()
                     <<", Error : pixel type not yet implemented ("
                    << id
                    << ")";
        }
    }
    return res;
}


medAbstractData * medMaskApplication::output()
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createMedMaskApplication()
{
    return new medMaskApplication;
}
