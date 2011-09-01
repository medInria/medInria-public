/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Alexandre <email>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "itkFilters.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include "itkImage.h"
#include "itkMedianImageFilter.h"
#include "itkNormalizeImageFilter.h"
#include "itkShrinkImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkAddConstantToImageFilter.h"
#include "itkMultiplyByConstantImageFilter.h"
#include "itkDivideByConstantImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"

class itkFiltersPrivate
{
public:
    dtkAbstractData *input;
    dtkAbstractData *output;
    double addValue;
    double multiplyValue;
    double divideValue;
    double sigmaValue;
    unsigned int shrinkFactors[3];

    itkFilters::FILTER filterType;
    template <class PixelType> int update ( void );
    template <class PixelType> void addFilter ( void );
    template <class PixelType> void multiplyFilter ( void );
    template <class PixelType> void divideFilter ( void );
    template <class PixelType> void gaussianFilter ( void );
    template <class PixelType> void normalizeFilter ( void );
    template <class PixelType> void medianFilter ( void );
    template <class PixelType> void invertFilter ( void );
    template <class PixelType> void shrinkFilter ( void );
};

template <class PixelType> int itkFiltersPrivate::update ( void )
{
    switch ( filterType )
    {
    case itkFilters::ADD:
        qDebug() << "Calling Add filter";
        qDebug() << "Add value : " << addValue;
        this->addFilter<PixelType>();
        break;
    case itkFilters::MULTIPLY:
        qDebug() << "Calling Multiply filter";
        qDebug() << "Multiply value : " << multiplyValue;
        this->multiplyFilter<PixelType>();
        break;
    case itkFilters::DIVIDE:
        qDebug() << "Calling Divide filter";
        qDebug() << "Divide value : " << divideValue;
        this->divideFilter<PixelType>();
        break;
    case itkFilters::GAUSSIAN:
        qDebug() << "Calling Gaussian filter";
        qDebug() << "Sigma : " << sigmaValue;
        this->gaussianFilter<PixelType>();
        break;
    case itkFilters::NORMALIZE:
        qDebug() << "Calling Normalize filter";
        this->normalizeFilter<PixelType>();
        break;
    case itkFilters::MEDIAN:
        qDebug() << "Calling Median filter";
        this->medianFilter<PixelType>();
        break;
    case itkFilters::INVERT:
        qDebug() << "Calling Invert intensity filter";
        this->invertFilter<PixelType>();
        break;
    case itkFilters::SHRINK:
        qDebug() << "Calling shrink filter";
        this->shrinkFilter<PixelType>();
        break;
    }

    return EXIT_SUCCESS;
}

template <class PixelType> void itkFiltersPrivate::addFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::AddConstantToImageFilter< ImageType, double, ImageType >  AddFilterType;
    typename AddFilterType::Pointer addFilter = AddFilterType::New();

    addFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    addFilter->SetConstant ( addValue );
    addFilter->Update();
    output->setData ( addFilter->GetOutput() );
}

template <class PixelType> void itkFiltersPrivate::multiplyFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::MultiplyByConstantImageFilter < ImageType, double, ImageType >  MultiplyFilterType;
    typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();

    multiplyFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    multiplyFilter->SetConstant ( multiplyValue );
    multiplyFilter->Update();
    output->setData ( multiplyFilter->GetOutput() );
}

template <class PixelType> void itkFiltersPrivate::divideFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::DivideByConstantImageFilter < ImageType, double, ImageType >  DivideFilterType;
    typename DivideFilterType::Pointer divideFilter = DivideFilterType::New();

    divideFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    divideFilter->SetConstant ( divideValue );
    divideFilter->Update();
    output->setData ( divideFilter->GetOutput() );
}

template <class PixelType> void itkFiltersPrivate::gaussianFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::SmoothingRecursiveGaussianImageFilter< ImageType, ImageType >  GaussianFilterType;
    typename GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();

    gaussianFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    gaussianFilter->SetSigma ( sigmaValue );
    gaussianFilter->Update();
    output->setData ( gaussianFilter->GetOutput() );
}

template <class PixelType> void itkFiltersPrivate::normalizeFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::NormalizeImageFilter < ImageType, ImageType >  NormalizeFilterType;
    typename NormalizeFilterType::Pointer normalizeFilter = NormalizeFilterType::New();

    normalizeFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    normalizeFilter->Update();
    output->setData ( normalizeFilter->GetOutput() );
}

template <class PixelType> void itkFiltersPrivate::medianFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::MedianImageFilter < ImageType, ImageType >  MedianFilterType;
    typename MedianFilterType::Pointer medianFilter = MedianFilterType::New();

    medianFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    medianFilter->Update();
    output->setData ( medianFilter->GetOutput() );
}

template <class PixelType> void itkFiltersPrivate::invertFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::InvertIntensityImageFilter < ImageType, ImageType >  InvertFilterType;
    typename InvertFilterType::Pointer invertFilter = InvertFilterType::New();

    invertFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    invertFilter->Update();
    output->setData ( invertFilter->GetOutput() );
}

template <class PixelType> void itkFiltersPrivate::shrinkFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::ShrinkImageFilter< ImageType, ImageType >  ShrinkFilterType;
    typename ShrinkFilterType::Pointer shrinkFilter = ShrinkFilterType::New();

    shrinkFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    shrinkFilter->SetShrinkFactors ( shrinkFactors );
    shrinkFilter->Update();
    output->setData ( shrinkFilter->GetOutput() );
}

itkFilters::itkFilters ( void ) : dtkAbstractProcess(), d ( new itkFiltersPrivate )
{
    d->output = NULL;
    d->addValue = 100.0;
    d->multiplyValue = 2.0;
    d->divideValue = 2.0;
    d->sigmaValue = 1.0;

    d->filterType = itkFilters::ADD;
}

itkFilters::~itkFilters ( void )
{
    delete d;
    d = NULL;
}

bool itkFilters::registered ( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType ( "itkFilters", createitkFilters );
}

QString itkFilters::description ( void ) const
{
    return "itkFilters";
}

void itkFilters::setInput ( dtkAbstractData *data )
{
    if ( !data )
        return;

    QString description = data->description();

    d->output = dtkAbstractDataFactory::instance()->create ( description );

    d->input = data;
}

void itkFilters::setParameter ( double  data, int channel )
{
    int tmpData = ( int ) data;
    switch ( channel )
    {
    case 0:
        switch ( tmpData )
        {
        case 0:
            d->filterType = itkFilters::ADD;
            break;
        case 1:
            d->filterType = itkFilters::MULTIPLY;
            break;
        case 2:
            d->filterType = itkFilters::DIVIDE;
            break;
        case 3:
            d->filterType = itkFilters::GAUSSIAN;
            break;
        case 4:
            d->filterType = itkFilters::NORMALIZE;
            break;
        case 5:
            d->filterType = itkFilters::MEDIAN;
            break;
        case 6:
            d->filterType = itkFilters::INVERT;
            break;
        case 7:
            d->filterType = itkFilters::SHRINK;
            break;
        }
        break;
    case 1 :
        switch ( d->filterType )
        {
        case itkFilters::ADD:
            d->addValue = data;
            break;
        case itkFilters::MULTIPLY:
            d->multiplyValue = data;
            break;
        case itkFilters::DIVIDE:
            d->divideValue = data;
            break;
        case itkFilters::GAUSSIAN:
            d->sigmaValue = data;
            break;
        case itkFilters::NORMALIZE:
            break;
        case itkFilters::MEDIAN:
            break;
        case itkFilters::INVERT:
            break;
        case itkFilters::SHRINK:
            d->shrinkFactors[0] = ( unsigned int ) data;
            break;
        }
        break;
    case 2:
        if ( d->filterType == itkFilters::SHRINK )
            d->shrinkFactors[1] = ( unsigned int ) data;
        break;
    case 3:
        if ( d->filterType == itkFilters::SHRINK )
            d->shrinkFactors[2] = ( unsigned int ) data;
        break;
    default :
        return;
    }
}

int itkFilters::update ( void )
{
    if ( !d->input )
        return -1;

    QString descr = d->input->description();

    qDebug() << "itkFilters, update : " << descr;

    if ( descr == "itkDataImageChar3" )
    {
        d->update<char>();
    }
    else if ( descr == "itkDataImageUChar3" )
    {
        d->update<unsigned char>();
    }
    else if ( descr == "itkDataImageShort3" )
    {
        d->update<short>();
    }
    else if ( descr == "itkDataImageUShort3" )
    {
        d->update<unsigned short>();
    }
    else if ( descr == "itkDataImageInt3" )
    {
        d->update<int>();
    }
    else if ( descr == "itkDataImageUInt3" )
    {
        d->update<unsigned int>();
    }
    else if ( descr == "itkDataImageLong3" )
    {
        d->update<long>();
    }
    else if ( descr== "itkDataImageULong3" )
    {
        d->update<unsigned long>();
    }
    else if ( descr == "itkDataImageFloat3" )
    {
        d->update<float>();
    }
    else if ( descr == "itkDataImageDouble3" )
    {
        d->update<double>();
    }
    else
    {
        qDebug() << "Error : pixel type not yet implemented ("
        << descr
        << ")";
        return -1;
    }

    return EXIT_SUCCESS;
}

dtkAbstractData * itkFilters::output ( void )
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFilters ( void )
{
    return new itkFilters;
}
