/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef ITKFILTERSADDPROCESS_H
#define ITKFILTERSADDPROCESS_H

#include <dtkCore/dtkAbstractData.h>
#include "itkFiltersProcessBase.h"

#include "itkFiltersPluginExport.h"

class itkFiltersAddProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersAddProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersAddProcess(itkFiltersAddProcess * parent = 0);
    itkFiltersAddProcess(const itkFiltersAddProcess& other);
    virtual ~itkFiltersAddProcess(void);

    static bool registered ( void );
    
public slots:

    void setParameter ( double  data, int channel );
    int update ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersAddProcess)
};

dtkAbstractProcess * createitkFiltersAddProcess(void);
#endif // ITKFILTERSADDPROCESS_H
