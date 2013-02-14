#ifndef ITKFILTERSGAUSSIANPROCESS_H
#define ITKFILTERSGAUSSIANPROCESS_H

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersGaussianProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersGaussianProcess : public dtkAbstractProcess
{
    Q_OBJECT
    
public:
    itkFiltersGaussianProcess(void);
    virtual ~itkFiltersGaussianProcess(void);
    
    virtual QString description ( void ) const;

    static bool registered ( void );

    void emitProgress(int progress);
    
public slots:

    void setInput ( dtkAbstractData *data );
    void setParameter ( double  data, int channel );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    itkFiltersGaussianProcessPrivate *d;
};


dtkAbstractProcess * createitkFiltersGaussianProcess(void);

#endif // ITKFILTERSGAUSSIANPROCESS_H
