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
#include <medAbstractData.h>

#include <itkFiltersPluginExport.h>

class itkMorphologicalFiltersProcessBasePrivate;

class ITKFILTERSPLUGIN_EXPORT itkMorphologicalFiltersProcessBase : public itkFiltersProcessBase
{
    Q_OBJECT
public:
    itkMorphologicalFiltersProcessBase(itkMorphologicalFiltersProcessBase * parent = 0);
    itkMorphologicalFiltersProcessBase(const itkMorphologicalFiltersProcessBase& other);
    virtual ~itkMorphologicalFiltersProcessBase(void);
    
public:
    void setParameter(double data, int channel);
    
private:
    DTK_DECLARE_PRIVATE(itkMorphologicalFiltersProcessBase)
};

