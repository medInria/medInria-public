#ifndef ITKFILTERSMULTIPLYPROCESS_P_H
#define ITKFILTERSMULTIPLYPROCESS_P_H


#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkMultiplyByConstantImageFilter.h"

class itkFiltersMultiplyProcess;

class itkFiltersMultiplyProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersMultiplyProcessPrivate(itkFiltersMultiplyProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersMultiplyProcessPrivate(const itkFiltersMultiplyProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersMultiplyProcessPrivate(void) {}
    
    double multiplyFactor;
    
    template <class PixelType> void update ( void )
    {
        this->setupFilter<PixelType>();
        this->setFilterDescription();
    }
    
    template <class PixelType> void setupFilter() 
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::MultiplyByConstantImageFilter< ImageType, double, ImageType >  MultiplyFilterType;
        typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();
    
        multiplyFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        multiplyFilter->SetConstant ( multiplyFactor );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersMultiplyProcessPrivate::eventCallback );
    
        multiplyFilter->AddObserver ( itk::ProgressEvent(), callback );
    
        multiplyFilter->Update();
        output->setData ( multiplyFilter->GetOutput() );
    }
    
    void setFilterDescription()
    {    
        //Set output description metadata
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " add filter (" + QString::number(multiplyFactor) + ")";
    
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersMultiplyProcess, itkFiltersProcessBase)

#endif // ITKFILTERSMULTIPLYPROCESS_P_H
