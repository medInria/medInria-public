#ifndef ITKFILTERSINVERTPROCESS_P_H
#define ITKFILTERSINVERTPROCESS_P_H


#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkInvertIntensityImageFilter.h"

class itkFiltersInvertProcess;

class itkFiltersInvertProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersInvertProcessPrivate(itkFiltersInvertProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersInvertProcessPrivate(const itkFiltersInvertProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersInvertProcessPrivate(void) {}
    
    double addValue;
    
    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::InvertIntensityImageFilter< ImageType, ImageType >  InvertFilterType;
        typename InvertFilterType::Pointer invertFilter = InvertFilterType::New();
    
        invertFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersInvertProcessPrivate::eventCallback );
    
        invertFilter->AddObserver ( itk::ProgressEvent(), callback );
    
        invertFilter->Update();
        output->setData ( invertFilter->GetOutput() );
        
        //Set output description metadata
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " invert filter";
    
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersInvertProcess, itkFiltersProcessBase)

#endif // ITKFILTERSINVERTPROCESS_P_H
