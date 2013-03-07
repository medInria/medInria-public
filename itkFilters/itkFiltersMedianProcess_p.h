#ifndef ITKFILTERSMEDIANPROCESS_P_H
#define ITKFILTERSMEDIANPROCESS_P_H

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkMedianImageFilter.h"

class itkFiltersMedianProcess;

class itkFiltersMedianProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersMedianProcessPrivate(itkFiltersMedianProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersMedianProcessPrivate(const itkFiltersMedianProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersMedianProcessPrivate(void) {}
    
    double addValue;
    
    template <class PixelType> void update ( void )
    {
        this->setupFilter<PixelType>();
        this->setFilterDescription();
    }
    
    template <class PixelType> void setupFilter() 
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::MedianImageFilter< ImageType, ImageType >  MedianFilterType;
        typename MedianFilterType::Pointer medianFilter = MedianFilterType::New();
    
        medianFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersMedianProcessPrivate::eventCallback );
    
        medianFilter->AddObserver ( itk::ProgressEvent(), callback );
    
        medianFilter->Update();
        output->setData ( medianFilter->GetOutput() );
    }
    
    void setFilterDescription()
    {    
        //Set output description metadata
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " median filter";
    
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersMedianProcess, itkFiltersProcessBase)

#endif // ITKFILTERSMEDIANPROCESS_P_H
