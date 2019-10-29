/*=========================================================================
 medInria
 Copyright (c) INRIA 2013 - 2019. All rights reserved.
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
    itkFiltersGrayscaleCloseProcess(itkFiltersGrayscaleCloseProcess *parent = nullptr);

    static bool registered();
    virtual QString description() const;
};

dtkAbstractProcess * createitkFiltersGrayscaleCloseProcess();
