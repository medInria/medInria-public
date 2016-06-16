/*=========================================================================
 medInria
 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#pragma once

#include <medAbstractData.h>
#include <itkMorphologicalFiltersProcessBase.h>

#include <itkFiltersPluginExport.h>

class itkMorphologicalFiltersProcessBasePrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersGrayscaleOpenProcess : public itkMorphologicalFiltersProcessBase
{
    Q_OBJECT

public:
    itkFiltersGrayscaleOpenProcess(itkFiltersGrayscaleOpenProcess * parent = 0);
    itkFiltersGrayscaleOpenProcess(const itkFiltersGrayscaleOpenProcess& other);
    virtual ~itkFiltersGrayscaleOpenProcess(void);

    static bool registered ( void );
};

dtkAbstractProcess * createitkFiltersGrayscaleOpenProcess(void);
