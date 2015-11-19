/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcess.h>
#include <medAbstractImageData.h>
#include <dtkLog/dtkLog.h>

#include <itkFiltersPluginExport.h>

#include <itkProcessObject.h>
#include <itkCommand.h>

class itkMorphologicalFiltersProcessBase;

class ITKFILTERSPLUGIN_EXPORT itkMorphologicalFiltersProcessBasePrivate : public medAbstractProcessPrivate
{
public:
    itkMorphologicalFiltersProcessBasePrivate(itkMorphologicalFiltersProcessBase *q = 0) : medAbstractProcessPrivate(q) {}
    itkMorphologicalFiltersProcessBasePrivate(const itkMorphologicalFiltersProcessBasePrivate& other) : medAbstractProcessPrivate(other) {}

    virtual ~itkMorphologicalFiltersProcessBasePrivate(void) {}
    
    QString description;
    QString filterType;
    
    itk::CStyleCommand::Pointer callback;
    itkMorphologicalFiltersProcessBase *filter;
    
    dtkSmartPointer <medAbstractImageData> input;
    dtkSmartPointer <medAbstractImageData> output;
	
	int radius[3];
	double radiusMm[3];
	bool isRadiusInPixels;
	int radiusInPixels;
    
    template <class PixelType> void setupFilter() {}
    virtual void setFilterDescription() {}
    
    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData) {
        itkMorphologicalFiltersProcessBasePrivate * source = reinterpret_cast<itkMorphologicalFiltersProcessBasePrivate *> ( clientData );
        itk::ProcessObject * processObject = ( itk::ProcessObject* ) caller;
    
        if ( !source ) { dtkWarn() << "Source is null"; }
    
        source->filter->emitProgress((int) (processObject->GetProgress() * 100));
        
    }
	
	template <class ImageType> void convertMmInPixels ( void )
	{
		ImageType *image = dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) );
		for (unsigned int i=0; i<image->GetSpacing().Size(); i++)
		{
			radius[i] = floor((radius[i]/image->GetSpacing()[i])+0.5); //rounding
			radiusMm[i] = radius[i] * image->GetSpacing()[i];
		}
	}

};

DTK_IMPLEMENT_PRIVATE(itkMorphologicalFiltersProcessBase, medAbstractProcess);


