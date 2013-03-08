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
    
    itkFiltersSubtractProcess& operator = (const itkFiltersSubtractProcess& other);
   
    virtual QString description(void) const;
    
    static bool registered(void);
    
public slots:
    void setInput(dtkAbstractData *data);
    void setParameter(double data, int channel);
    int update(void);
    
    dtkAbstractData *output(void);
    
private:
    DTK_DECLARE_PRIVATE(itkFiltersSubtractProcess)
};


dtkAbstractProcess * createitkFiltersSubtractProcess(void);

#endif // ITKFILTERSSUBTRACTPROCESS_H
