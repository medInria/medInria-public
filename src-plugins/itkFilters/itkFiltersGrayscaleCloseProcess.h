/*=========================================================================
 medInria
 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#pragma once

#include <itkMorphologicalFiltersProcessBase.h>

class ITKFILTERSPLUGIN_EXPORT itkFiltersGrayscaleCloseProcess : public itkMorphologicalFiltersProcessBase
{
    Q_OBJECT

public:
    itkFiltersGrayscaleCloseProcess(itkFiltersGrayscaleCloseProcess * parent = 0);
    itkFiltersGrayscaleCloseProcess(const itkFiltersGrayscaleCloseProcess& other);
    static bool registered ( void );
};

dtkAbstractProcess * createitkFiltersGrayscaleCloseProcess(void);
