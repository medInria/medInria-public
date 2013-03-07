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
    
    itkFiltersAddProcess& operator = (const itkFiltersAddProcess& other);
    
    virtual QString description ( void ) const;

    static bool registered ( void );
    
public slots:

    void setInput ( dtkAbstractData *data );
    void setParameter ( double  data, int channel );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersAddProcess)
};

dtkAbstractProcess * createitkFiltersAddProcess(void);
#endif // ITKFILTERSADDPROCESS_H
