#ifndef ITKFILTERSSUBTRACTPROCESS_H
#define ITKFILTERSSUBTRACTPROCESS_H

#include <dtkCore/dtkAbstractData.h>
#include "itkFiltersProcessBase.h"

#include "itkFiltersPluginExport.h"

class dtkAbstractData;
class itkFiltersSubtractProcessPrivate;

class ITKFILTERSPLUGIN_EXPORT itkFiltersSubtractProcess : public itkFiltersProcessBase
{
    Q_OBJECT
public:
    itkFiltersSubtractProcess(itkFiltersSubtractProcess * parent = 0);
    itkFiltersSubtractProcess(const itkFiltersSubtractProcess& other);
    virtual ~itkFiltersSubtractProcess(void);
    
    static bool registered(void);
    
public slots:
    void setParameter(double data, int channel);
    int update(void);
    
private:
    DTK_DECLARE_PRIVATE(itkFiltersSubtractProcess)
};


dtkAbstractProcess * createitkFiltersSubtractProcess(void);

#endif // ITKFILTERSSUBTRACTPROCESS_H
