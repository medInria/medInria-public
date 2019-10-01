//=========================================================================
//
// medInria
//
// Copyright (c) INRIA 2013 - 2019. All rights reserved.
// See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//=========================================================================

#pragma once

#include <itkMinimumMaximumImageCalculator.h>
#include <itkScalarImageToHistogramGenerator.h>
#include <itkImageDuplicator.h>

template <unsigned DIM,typename T>
struct itkDataImagePrivateTypeBase {
    typedef typename itk::Image<T,DIM> ImageType;

    itkDataImagePrivateTypeBase(): image(nullptr) { }
    itkDataImagePrivateTypeBase(const itkDataImagePrivateTypeBase<DIM,T>& other)
    {
        // create the filter that duplicates an image
        auto duplicator = itk::ImageDuplicator<ImageType>::New();
        duplicator->SetInputImage(other.image);
        duplicator->Update();
        this->image = duplicator->GetOutput();
        this->thumbnails = other.thumbnails;
    }

    typename ImageType::Pointer image;
    QList<QImage>               thumbnails;
};

template <unsigned DIM,typename T>
class itkDataVectorImagePrivateType: public itkDataImagePrivateTypeBase<DIM,T> {
public:

    itkDataVectorImagePrivateType(): itkDataImagePrivateTypeBase<DIM,T>() { }
    itkDataVectorImagePrivateType(const itkDataImagePrivateTypeBase<DIM,T>& other): itkDataImagePrivateTypeBase<DIM,T>(other) { }

    void reset() const { }

    int minRangeValue() const { return -1; }
    int maxRangeValue() const { return -1; }

    int scalarValueCount(int) const { return -1; }
    int scalarValueMinCount() const { return -1; }
    int scalarValueMaxCount() const { return -1; }
};

template <unsigned DIM,typename T>
class itkDataScalarImagePrivateType: public itkDataImagePrivateTypeBase<DIM,T> {

    typedef itkDataImagePrivateTypeBase<DIM,T> base;

public:

    typedef T                                                                    PixelType;
    typedef typename base::ImageType                                             ImageType;
    typedef typename itk::Statistics::ScalarImageToHistogramGenerator<ImageType> HistogramGeneratorType;
    typedef typename HistogramGeneratorType::HistogramType                       HistogramType;

    itkDataScalarImagePrivateType(): itkDataImagePrivateTypeBase<DIM,T>(), histogram(nullptr),range_min(0),range_max(0),histogram_min(0),histogram_max(0) {
        reset();
    }
    itkDataScalarImagePrivateType(const itkDataScalarImagePrivateType<DIM,T>& other): itkDataImagePrivateTypeBase<DIM,T>(other)
    {
        this->histogram = other.histogram;
        this->range_computed = other.range_computed;
        this->range_min = other.range_min;
        this->range_max = other.range_max;
        this->histogram_min = other.histogram_min;
        this->histogram_min = other.histogram_min;
        if (other.histogram)
        {
            this->histogram = dynamic_cast<HistogramType*>(other.histogram->CreateAnother().GetPointer());
        }
        else
        {
            this->histogram = nullptr;
        }
    }

    void reset() { range_computed = false; }

    int minRangeValue() {
        computeRange();
        if (!range_computed)
        {
            qDebug() << "Cannot compute range";
        }
        return range_min;
    }

    int maxRangeValue() {
        computeRange();
        return range_max;
    }

    int scalarValueCount(int value) {
        computeValueCounts();
        return histogram->GetFrequency((PixelType)value);
    }

    int scalarValueMinCount() {
        computeValueCounts();
        return histogram_min;
    }

    int scalarValueMaxCount() {
        computeValueCounts();
        return histogram_max;
    }

private:

    typename HistogramType::Pointer histogram;
    bool                            range_computed;
    PixelType                       range_min;
    PixelType                       range_max;
    int                             histogram_min;
    int                             histogram_max;

    void computeRange();
    void computeValueCounts();
};

template <unsigned DIM,typename T>
void itkDataScalarImagePrivateType<DIM,T>::computeRange() {
    if (range_computed)
        return;

    if (base::image->GetLargestPossibleRegion().GetNumberOfPixels()==0)
        return;

    typedef itk::MinimumMaximumImageCalculator<ImageType> MinMaxCalculatorType;

    typename MinMaxCalculatorType::Pointer calculator = MinMaxCalculatorType::New();
    calculator->SetImage(base::image);
    calculator->SetRegion(base::image->GetLargestPossibleRegion());
    try {
        calculator->Compute();
    } catch (itk::ExceptionObject &e) {
        std::cerr << e;
        return;
    }
    range_min = calculator->GetMinimum();
    range_max = calculator->GetMaximum();
    range_computed = true;
}

template <unsigned DIM,typename T>
void itkDataScalarImagePrivateType<DIM,T>::computeValueCounts() {
    if (histogram.IsNull()) {
        computeRange();

        typename HistogramGeneratorType::Pointer histogramGenerator = HistogramGeneratorType::New();
        histogramGenerator->SetInput(base::image);
        histogramGenerator->SetNumberOfBins(range_max-range_min+1);
        histogramGenerator->SetMarginalScale(1.0);
        histogramGenerator->SetHistogramMin(range_min);
        histogramGenerator->SetHistogramMax(range_max);

        try {
            std::cerr << "calculating histogram...";
            histogramGenerator->Compute();
            std::cerr << "done" << std::endl;
        } catch (itk::ExceptionObject &e) {
            std::cerr << e;
            return;
        }

        typedef typename HistogramGeneratorType::HistogramType HistogramType;
        typedef typename HistogramType::AbsoluteFrequencyType FrequencyType;

        histogram = const_cast<HistogramType*>(histogramGenerator->GetOutput());

        FrequencyType min = itk::NumericTraits< FrequencyType >::max();
        FrequencyType max = itk::NumericTraits< FrequencyType >::min();

        typedef typename HistogramType::Iterator Iterator;
        for (Iterator it=histogram->Begin(),end=histogram->End();it!=end;++it) {
            FrequencyType c = it.GetFrequency();
            if (min>c) min = c;
            if (max<c) max = c;
        }
        histogram_min = static_cast<int>(min);
        histogram_max = static_cast<int>(max);
    }
}
