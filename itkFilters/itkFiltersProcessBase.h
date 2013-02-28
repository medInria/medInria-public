#ifndef ITKFILTERSPROCESSBASE_H
#define ITKFILTERSPROCESSBASE_H

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class ITKFILTERSPLUGIN_EXPORT itkFiltersProcessBase : public dtkAbstractProcess
{
    Q_OBJECT
public:
    itkFiltersProcessBase(void);
    virtual ~itkFiltersProcessBase(void);
    
    void emitProgress(int progress);
};

#endif // ITKFILTERSPROCESSBASE_H
