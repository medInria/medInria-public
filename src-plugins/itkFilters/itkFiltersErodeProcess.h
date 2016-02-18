/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractData.h>
#include <itkMorphologicalFiltersProcessBase.h>

#include <itkFiltersPluginExport.h>

class itkFiltersErodeProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersErodeProcess : public itkMorphologicalFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersErodeProcess(itkFiltersErodeProcess * parent = 0);
    itkFiltersErodeProcess(const itkFiltersErodeProcess& other);
    virtual ~itkFiltersErodeProcess(void);

    static bool registered ( void );
    
public slots:
    int update ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersErodeProcess)
};

dtkAbstractProcess * createitkFiltersErodeProcess(void);
