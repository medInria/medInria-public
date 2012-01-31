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

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkIntensityWindowingImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkNormalizeImageFilter.h"
#include "itkShrinkImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkAddConstantToImageFilter.h"
#include "itkSubtractConstantFromImageFilter.h"
#include "itkMultiplyByConstantImageFilter.h"
#include "itkDivideByConstantImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"

class itkFiltersPrivate
{
public:
    itkFilters * filters;
    itk::CStyleCommand::Pointer callback;

    dtkAbstractData *input;
    dtkAbstractData *output;
    double addValue;
    double subtractValue;
    double multiplyValue;
    double divideValue;
    double sigmaValue;

    double minimumIntensityValue;
    double maximumIntensityValue;
    double minimumOutputIntensityValue;
    double maximumOutputIntensityValue;

    unsigned int shrinkFactors[3];

    itkFilters::FILTER filterType;
    template <class PixelType> int update ( void );
    template <class PixelType> void addFilter ( void );
    template <class PixelType> void subtractFilter ( void );
    template <class PixelType> void multiplyFilter ( void );
    template <class PixelType> void divideFilter ( void );
    template <class PixelType> void gaussianFilter ( void );
    template <class PixelType> void normalizeFilter ( void );
    template <class PixelType> void medianFilter ( void );
    template <class PixelType> void invertFilter ( void );
    template <class PixelType> void shrinkFilter ( void );
    template <class PixelType> void intensityFilter ( void );

    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData );
};


 void itkFiltersPrivate::eventCallback ( itk::Object* caller, const itk::EventObject& event, void* clientData )
 {
     itkFiltersPrivate * source = reinterpret_cast<itkFiltersPrivate *> ( clientData );
     itk::ProcessObject * processObject = ( itk::ProcessObject* ) caller;

//     qDebug() << (int) (processObject->GetProgress() * 100);

     if ( !source )
         qDebug() << "Source est null";

     source->filters->emitProgress((int) (processObject->GetProgress() * 100));
 }

template <class PixelType> int itkFiltersPrivate::update ( void )
{

     callback = itk::CStyleCommand::New();
     callback->SetClientData ( ( void * ) this );
     callback->SetCallback ( itkFiltersPrivate::eventCallback );

    switch ( filterType )
    {
    case itkFilters::ADD:
        qDebug() << "Calling Add filter";
        qDebug() << "Add value : " << addValue;
        this->addFilter<PixelType>();
        break;
    case itkFilters::SUBTRACT:
        qDebug() << "Calling Subtract filter";
        qDebug() << "Subtract value : " << subtractValue;
        this->subtractFilter<PixelType>();
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
    case itkFilters::INTENSITY:
        qDebug() << "Calling intensity window image filter";
        this->intensityFilter<PixelType>();
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

    addFilter->AddObserver ( itk::ProgressEvent(), callback );

    addFilter->Update();
    output->setData ( addFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " add filter (" + QString::number(addValue) + ")";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

template <class PixelType> void itkFiltersPrivate::subtractFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::SubtractConstantFromImageFilter< ImageType, double, ImageType >  SubtractFilterType;
    typename SubtractFilterType::Pointer subtractFilter = SubtractFilterType::New();

    qDebug() << "Subtract parameter (itkfilter) : " << subtractValue;

    subtractFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    subtractFilter->SetConstant ( subtractValue );

    subtractFilter->AddObserver ( itk::ProgressEvent(), callback );

    subtractFilter->Update();
    output->setData ( subtractFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " subtract filter (" + QString::number(subtractValue) + ")";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

template <class PixelType> void itkFiltersPrivate::multiplyFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::MultiplyByConstantImageFilter < ImageType, double, ImageType >  MultiplyFilterType;
    typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();

    multiplyFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    multiplyFilter->SetConstant ( multiplyValue );

    multiplyFilter->AddObserver ( itk::ProgressEvent(), callback );

    multiplyFilter->Update();
    output->setData ( multiplyFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " multiply filter (" + QString::number(multiplyValue) + ")";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

template <class PixelType> void itkFiltersPrivate::divideFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::DivideByConstantImageFilter < ImageType, double, ImageType >  DivideFilterType;
    typename DivideFilterType::Pointer divideFilter = DivideFilterType::New();

    divideFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    divideFilter->SetConstant ( divideValue );

    divideFilter->AddObserver ( itk::ProgressEvent(), callback );

    divideFilter->Update();
    output->setData ( divideFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " divide filter (" + QString::number(divideValue) + ")";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

template <class PixelType> void itkFiltersPrivate::gaussianFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::SmoothingRecursiveGaussianImageFilter< ImageType, ImageType >  GaussianFilterType;
    typename GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();

    gaussianFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    gaussianFilter->SetSigma ( sigmaValue );

    gaussianFilter->AddObserver ( itk::ProgressEvent(), callback );

    gaussianFilter->Update();
    output->setData ( gaussianFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " gaussian filter (Sigma=" + QString::number(sigmaValue) + ")";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

template <class PixelType> void itkFiltersPrivate::normalizeFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::NormalizeImageFilter < ImageType, ImageType >  NormalizeFilterType;
    typename NormalizeFilterType::Pointer normalizeFilter = NormalizeFilterType::New();

    normalizeFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );

    normalizeFilter->AddObserver ( itk::ProgressEvent(), callback );

    normalizeFilter->Update();
    output->setData ( normalizeFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " normalize filter";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

template <class PixelType> void itkFiltersPrivate::medianFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::MedianImageFilter < ImageType, ImageType >  MedianFilterType;
    typename MedianFilterType::Pointer medianFilter = MedianFilterType::New();

    medianFilter->AddObserver ( itk::ProgressEvent(), callback );

    medianFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    medianFilter->Update();
    output->setData ( medianFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " median filter";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

template <class PixelType> void itkFiltersPrivate::invertFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::InvertIntensityImageFilter < ImageType, ImageType >  InvertFilterType;
    typename InvertFilterType::Pointer invertFilter = InvertFilterType::New();

    invertFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );

    invertFilter->AddObserver ( itk::ProgressEvent(), callback );

    invertFilter->Update();
    output->setData ( invertFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " invert filter";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

template <class PixelType> void itkFiltersPrivate::shrinkFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::ShrinkImageFilter< ImageType, ImageType >  ShrinkFilterType;
    typename ShrinkFilterType::Pointer shrinkFilter = ShrinkFilterType::New();

    shrinkFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
    shrinkFilter->SetShrinkFactors ( shrinkFactors );

    shrinkFilter->AddObserver ( itk::ProgressEvent(), callback );

    shrinkFilter->Update();
    output->setData ( shrinkFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " shrink filter (" + QString::number(shrinkFactors[0]) + "," + QString::number(shrinkFactors[1]) + "," + QString::number(shrinkFactors[2]) + ")";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

template <class PixelType> void itkFiltersPrivate::intensityFilter ( void )
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::IntensityWindowingImageFilter< ImageType, ImageType >  IntensityFilterType;
    typename IntensityFilterType::Pointer intensityFilter = IntensityFilterType::New();

    intensityFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );

    intensityFilter->SetWindowMinimum ( ( PixelType ) minimumIntensityValue );
    intensityFilter->SetWindowMaximum ( ( PixelType ) maximumIntensityValue );
    intensityFilter->SetOutputMinimum ( ( PixelType ) minimumOutputIntensityValue );
    intensityFilter->SetOutputMaximum ( ( PixelType ) maximumOutputIntensityValue );

    intensityFilter->AddObserver ( itk::ProgressEvent(), callback );

    intensityFilter->Update();
    output->setData ( intensityFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " intensity filter";

    output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
}

itkFilters::itkFilters ( void ) : dtkAbstractProcess(), d ( new itkFiltersPrivate )
{
    d->filters = this;

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

void itkFilters::emitProgress(int prog)
{
    emit progressed(prog);
}

bool itkFilters::registered ( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType ( "itkFilters", createitkFilters );
}

QString itkFilters::description ( void ) const
{
    return tr("Simple itk filters, operating on a single volume");
}


void itkFilters::setInput ( dtkAbstractData *data )
{
    if ( !data )
        return;

    QString identifier = data->identifier();

    d->output = dtkAbstractDataFactory::instance()->create ( identifier );

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
            d->filterType = itkFilters::SUBTRACT;
            break;
        case 2:
            d->filterType = itkFilters::MULTIPLY;
            break;
        case 3:
            d->filterType = itkFilters::DIVIDE;
            break;
        case 4:
            d->filterType = itkFilters::GAUSSIAN;
            break;
        case 5:
            d->filterType = itkFilters::NORMALIZE;
            break;
        case 6:
            d->filterType = itkFilters::MEDIAN;
            break;
        case 7:
            d->filterType = itkFilters::INVERT;
            break;
        case 8:
            d->filterType = itkFilters::SHRINK;
            break;
        case 9:
            d->filterType = itkFilters::INTENSITY;
        }
        break;
    case 1 :
        switch ( d->filterType )
        {
        case itkFilters::ADD:
            d->addValue = data;
            break;
        case itkFilters::SUBTRACT:
            d->subtractValue = data;
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
        case itkFilters::INTENSITY:
            d->minimumIntensityValue = data;
            break;
        }
        break;
    case 2:
        if ( d->filterType == itkFilters::SHRINK )
            d->shrinkFactors[1] = ( unsigned int ) data;
        if ( d->filterType == itkFilters::INTENSITY )
            d->maximumIntensityValue = data;
        break;
    case 3:
        if ( d->filterType == itkFilters::SHRINK )
            d->shrinkFactors[2] = ( unsigned int ) data;
        if ( d->filterType == itkFilters::INTENSITY )
            d->minimumOutputIntensityValue = data;
        break;
    case 4:
        if ( d->filterType == itkFilters::INTENSITY )
            d->maximumOutputIntensityValue = data;
        break;
    default :
        return;
    }
}

int itkFilters::update ( void )
{
    if ( !d->input )
        return -1;

    QString id = d->input->identifier();

    qDebug() << "itkFilters, update : " << id;

    if ( id == "itkDataImageChar3" )
    {
        d->update<char>();
    }
    else if ( id == "itkDataImageUChar3" )
    {
        d->update<unsigned char>();
    }
    else if ( id == "itkDataImageShort3" )
    {
        d->update<short>();
    }
    else if ( id == "itkDataImageUShort3" )
    {
        d->update<unsigned short>();
    }
    else if ( id == "itkDataImageInt3" )
    {
        d->update<int>();
    }
    else if ( id == "itkDataImageUInt3" )
    {
        d->update<unsigned int>();
    }
    else if ( id == "itkDataImageLong3" )
    {
        d->update<long>();
    }
    else if ( id== "itkDataImageULong3" )
    {
        d->update<unsigned long>();
    }
    else if ( id == "itkDataImageFloat3" )
    {
        d->update<float>();
    }
    else if ( id == "itkDataImageDouble3" )
    {
        d->update<double>();
    }
    else
    {
        qDebug() << "Error : pixel type not yet implemented ("
        << id
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
