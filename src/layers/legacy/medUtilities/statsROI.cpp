/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 *
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "statsROI.h"

#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medAttachedData.h>
#include <medDataManager.h>

#include <itkImageRegionIterator.h>
#include <itkMinimumMaximumImageCalculator.h>

#include <vector>
#include <numeric>
#include <functional>

// /////////////////////////////////////////////////////////////////
// statsROIInternal
// /////////////////////////////////////////////////////////////////

// The logic behind this class is a bit strange - for some reason, on Mac 0SX Lion and earlier,
// *some* template code declared in the header and defined in the body results in a symbol clash (probably).
// Behaviour is that the template function is never called, *unless* the template is specialized.
// Moving the templated code here, to a private class, fixes the problem. I have no idea why. MJB

class statsROIInternal
{

public:
    // Rather than inherit, because we don't need to, we'll just do some composition
    // If the access qualifiers change on statsROI, remember to change this!
    statsROIInternal(statsROI* statsROIObject) : composite(statsROIObject) { }

    // Run the Stats process
    template <class ImageType> int runStats()
    {
        int res = DTK_FAILURE;

        if (composite->chooseFct == statsROI::MEAN_STDDEVIATION) // DEFAULT: Compute Mean and Standard deviation
        {
            if (composite->input1)
            {
                QString id = composite->input1->identifier();

                if ( id == "itkDataImageChar3" )
                {
                    res = runMeanStdDeviation< ImageType, itk::Image <char,3> >();
                }
                else if ( id == "itkDataImageUChar3" )
                {
                    res = runMeanStdDeviation< ImageType, itk::Image <unsigned char,3> >();
                }
                else if ( id == "itkDataImageShort3" )
                {
                    res = runMeanStdDeviation< ImageType, itk::Image <short,3> >();
                }
                else if ( id == "itkDataImageUShort3" )
                {
                    res = runMeanStdDeviation< ImageType, itk::Image <unsigned short,3> >();
                }
                else if ( id == "itkDataImageInt3" )
                {
                    res = runMeanStdDeviation< ImageType, itk::Image <int,3> >();
                }
                else if ( id == "itkDataImageUInt3" )
                {
                    res = runMeanStdDeviation< ImageType, itk::Image <unsigned int,3> >();
                }
                else if ( id == "itkDataImageLong3" )
                {
                    res = runMeanStdDeviation< ImageType, itk::Image <long,3> >();
                }
                else if ( id== "itkDataImageULong3" )
                {
                    res = runMeanStdDeviation< ImageType, itk::Image <unsigned long,3> >();
                }
                else if ( id == "itkDataImageFloat3" )
                {
                    res = runMeanStdDeviation< ImageType, itk::Image <float,3> >();
                }
                else if ( id == "itkDataImageDouble3" )
                {
                    res = runMeanStdDeviation< ImageType, itk::Image <double,3> >();
                }
                else
                {
                    qDebug() <<"statsROI, error: pixel type not yet implemented ("
                             << id
                             << ")";
                }
            }
        }
        else // Compute Volume in mL
        {
            if (composite->chooseFct == statsROI::VOLUMEML)
            {
                res = runVolumeML<ImageType>();
            }
            else if (composite->chooseFct == statsROI::MINMAX)
            {
                res = runMinMax<ImageType>();
            }
        }
        return res;

    }

    template <class ImageType, class ImageType2> int runMeanStdDeviation()
    {
        typename ImageType::Pointer  imag = dynamic_cast<ImageType  *> ( ( itk::Object* ) ( composite->input0->data() )) ;
        typename ImageType2::Pointer mask = dynamic_cast<ImageType2 *> ( ( itk::Object* ) ( composite->input1->data() )) ;

        // Iterators
        typedef itk::ImageRegionIterator <ImageType> MaskIterator;
        MaskIterator iti(imag, imag->GetBufferedRegion());
        typedef itk::ImageRegionIterator <ImageType2> MaskIterator2;
        MaskIterator2 itm(mask, mask->GetBufferedRegion());

        std::vector<double> v;

        // Loop over the mask
        for (itm.GoToBegin(); !itm.IsAtEnd(); ++itm)
        {
            if(itm.Get())
                v.push_back((double)iti.Get());
            ++iti;
        }

        double sum = std::accumulate(v.begin(), v.end(), 0.0);
        double mean =  sum / v.size();

        std::vector<double> diff(v.size());
        std::transform(v.begin(), v.end(), diff.begin(), [mean](double dDiffVal) {return dDiffVal - mean; });

        double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        double stdev = std::sqrt(sq_sum / v.size());

        composite->computedOutput.push_back(mean);
        composite->computedOutput.push_back(stdev);

        return DTK_SUCCEED;
    }

    template <class ImageType> int runVolumeML()
    {
        typename ImageType::Pointer m_itkMask = dynamic_cast<ImageType *> ( static_cast<itk::Object*> ( composite->input0->data() ));

        typename ImageType::SizeType size = m_itkMask->GetLargestPossibleRegion().GetSize();
        typename ImageType::IndexType px;
        int nbPixInMask = 0;
        for (unsigned int sl=0 ; sl<size[2] ; sl++)
        {
            for (unsigned int y=0 ; y<size[1] ; y++)
            {
                for (unsigned int x=0 ; x<size[0] ; x++)
                {
                    px[0] = x;
                    px[1] = y;
                    px[2] = sl;
                    if (m_itkMask->GetPixel(px)!=composite->outsideValue)
                    {
                        nbPixInMask++;
                    }
                }
            }
        }
        double volumeInMm3 =
                nbPixInMask * m_itkMask->GetSpacing()[0]*m_itkMask->GetSpacing()[1]*m_itkMask->GetSpacing()[2];

        composite->computedOutput.push_back(volumeInMm3/1000.);

        return DTK_SUCCEED;
    }

    template <class ImageType> int runMinMax()
    {
        typedef itk::MinimumMaximumImageCalculator <ImageType> ImageCalculatorFilterType;
        typename ImageCalculatorFilterType::Pointer imageCalculatorFilter
                = ImageCalculatorFilterType::New ();

        typename ImageType::Pointer imgInput = dynamic_cast<ImageType *> ( ( itk::Object* ) ( composite->input0->data() )) ;

        imageCalculatorFilter->SetImage(imgInput);
        imageCalculatorFilter->Compute();

        composite->computedOutput.push_back(imageCalculatorFilter->GetMinimum());
        composite->computedOutput.push_back(imageCalculatorFilter->GetMaximum());

        return DTK_SUCCEED;
    }

private:
    statsROI* const composite;
};


// /////////////////////////////////////////////////////////////////
// statsROI
// /////////////////////////////////////////////////////////////////

statsROI::statsROI()
{
    chooseFct = MEAN_STDDEVIATION;
    outsideValue = 0;
}

void statsROI::setInput (medAbstractData *data, int channel)
{
    if ( !data ) return;

    if (!channel)
    {
        this->input0 = data; //channel 0 = data
    }
    else
    {
        this->input1 = data; //channel 1 = mask if needed
    }
}

void statsROI::setParameter(statsParameter fct)
{
    chooseFct = fct;
}

void statsROI::setParameter(double outsideValue)
{
    this->outsideValue = outsideValue;
}

// Convert medAbstractData to ITK volume
int statsROI::update()
{
    int res = DTK_FAILURE;
    statsROIInternal internalHandler(this);

    if (this->input0)
    {
        QString id = this->input0->identifier();

        if ( id == "itkDataImageChar3" )
        {
            res = internalHandler.runStats< itk::Image <char,3> >();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            res = internalHandler.runStats< itk::Image <unsigned char,3> >();
        }
        else if ( id == "itkDataImageShort3" )
        {
            res = internalHandler.runStats< itk::Image <short,3> >();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            res = internalHandler.runStats< itk::Image <unsigned short,3> >();
        }
        else if ( id == "itkDataImageInt3" )
        {
            res = internalHandler.runStats< itk::Image <int,3> >();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            res = internalHandler.runStats< itk::Image <unsigned int,3> >();
        }
        else if ( id == "itkDataImageLong3" )
        {
            res = internalHandler.runStats< itk::Image <long,3> >();
        }
        else if ( id== "itkDataImageULong3" )
        {
            res = internalHandler.runStats< itk::Image <unsigned long,3> >();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            res = internalHandler.runStats< itk::Image <float,3> >();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            res = internalHandler.runStats< itk::Image <double,3> >();
        }
        else
        {
            qDebug() << "statsROI, error: pixel type not yet implemented ("
                     << id
                     << ")";
        }
    }
    return res;
}

std::vector<double> statsROI::output()
{
    return this->computedOutput;
}
