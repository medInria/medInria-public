#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMorphologicalFiltersProcessBase.h>

class ITKFILTERSPLUGIN_EXPORT itkFiltersDilateProcess : public itkMorphologicalFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersDilateProcess(itkFiltersDilateProcess *parent = nullptr);
    static bool registered();
    virtual QString description() const;
};

dtkAbstractProcess * createitkFiltersDilateProcess();
