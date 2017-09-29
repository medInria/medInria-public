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

class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersNormalizeProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersNormalizeProcess(itkFiltersNormalizeProcess * parent = 0);

    static bool registered ( void );

    virtual QString description(void) const;
    
public slots:

    int tryUpdate();

protected:
    template <class PixelType> int updateProcess();
};

dtkAbstractProcess * createitkFiltersNormalizeProcess(void);



