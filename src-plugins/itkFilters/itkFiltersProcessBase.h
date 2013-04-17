/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef ITKFILTERSPROCESSBASE_H
#define ITKFILTERSPROCESSBASE_H

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersProcessBasePrivate;

class ITKFILTERSPLUGIN_EXPORT itkFiltersProcessBase : public dtkAbstractProcess
{
    Q_OBJECT
public:
    itkFiltersProcessBase(itkFiltersProcessBase * parent = 0);
    itkFiltersProcessBase(const itkFiltersProcessBase& other);
    virtual ~itkFiltersProcessBase(void);
    
public:
    itkFiltersProcessBase& operator = (const itkFiltersProcessBase& other);

    QString description ( void );
    
    void setInput ( dtkAbstractData *data );
    dtkAbstractData *output ( void );
    
    void emitProgress(int progress);
    
private:
    DTK_DECLARE_PRIVATE(itkFiltersProcessBase)
};

//ITKFILTERSPLUGIN_EXPORT QDebug operator<<(QDebug debug, const itkFiltersProcessBase& process);
//ITKFILTERSPLUGIN_EXPORT QDebug operator<<(QDebug debug,       itkFiltersProcessBase *process);


#endif // ITKFILTERSPROCESSBASE_H
