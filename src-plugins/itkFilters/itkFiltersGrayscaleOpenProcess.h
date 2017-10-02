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

class ITKFILTERSPLUGIN_EXPORT itkFiltersGrayscaleOpenProcess : public itkMorphologicalFiltersProcessBase
{
    Q_OBJECT

public:
    itkFiltersGrayscaleOpenProcess(itkFiltersGrayscaleOpenProcess * parent = 0);

    static bool registered ( void );

    virtual QString description(void) const;
};

dtkAbstractProcess * createitkFiltersGrayscaleOpenProcess(void);
