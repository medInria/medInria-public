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

class itkFiltersDilateProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersDilateProcess : public itkMorphologicalFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersDilateProcess(itkFiltersDilateProcess * parent = 0);
    itkFiltersDilateProcess(const itkFiltersDilateProcess& other);
    virtual ~itkFiltersDilateProcess(void);

    static bool registered ( void );
    
public slots:
    int update ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersDilateProcess)
};

dtkAbstractProcess * createitkFiltersDilateProcess(void);
