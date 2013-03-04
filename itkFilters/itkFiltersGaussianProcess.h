#ifndef ITKFILTERSGAUSSIANPROCESS_H
#define ITKFILTERSGAUSSIANPROCESS_H

#include "itkFiltersProcessBase.h"
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersGaussianProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersGaussianProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersGaussianProcess(itkFiltersGaussianProcess * parent = 0);
    itkFiltersGaussianProcess(const itkFiltersGaussianProcess& other);
    virtual ~itkFiltersGaussianProcess(void);
    
    itkFiltersGaussianProcess& operator = (const itkFiltersGaussianProcess& other);

    virtual QString description ( void ) const;

    static bool registered ( void );
    
public slots:

    void setInput ( dtkAbstractData *data );
    void setParameter ( double  data, int channel );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersGaussianProcess)
};


ITKFILTERSPLUGIN_EXPORT QDebug operator<<(QDebug debug, const itkFiltersGaussianProcess& process);
ITKFILTERSPLUGIN_EXPORT QDebug operator<<(QDebug debug,       itkFiltersGaussianProcess *process);

dtkAbstractProcess * createitkFiltersGaussianProcess(void);

#endif // ITKFILTERSGAUSSIANPROCESS_H
