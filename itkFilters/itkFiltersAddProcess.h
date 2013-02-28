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
    itkFiltersAddProcess(void);
    virtual ~itkFiltersAddProcess(void);
    
    virtual QString description ( void ) const;

    static bool registered ( void );
    
public slots:

    void setInput ( dtkAbstractData *data );
    void setParameter ( double  data, int channel );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    itkFiltersAddProcessPrivate *d;
};


dtkAbstractProcess * createitkFiltersAddProcess(void);
#endif // ITKFILTERSADDPROCESS_H
