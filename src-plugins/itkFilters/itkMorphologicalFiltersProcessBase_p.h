/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkFiltersProcessBase.h>
#include <itkFiltersProcessBase_p.h>
#include <medAbstractImageData.h>
#include <dtkLog/dtkLog.h>

#include <itkFiltersPluginExport.h>

#include <itkProcessObject.h>
#include <itkCommand.h>

class itkMorphologicalFiltersProcessBase;

class ITKFILTERSPLUGIN_EXPORT itkMorphologicalFiltersProcessBasePrivate : public itkFiltersProcessBasePrivate
{
public:
    itkMorphologicalFiltersProcessBasePrivate(itkMorphologicalFiltersProcessBase *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkMorphologicalFiltersProcessBasePrivate(const itkMorphologicalFiltersProcessBasePrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkMorphologicalFiltersProcessBasePrivate(void) {}

    int radius[3];
    double radiusMm[3];
    bool isRadiusInPixels;
    int radiusInPixels;
	
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

DTK_IMPLEMENT_PRIVATE(itkMorphologicalFiltersProcessBase, itkFiltersProcessBase);


