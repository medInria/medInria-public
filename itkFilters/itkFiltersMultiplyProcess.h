#ifndef ITKFILTERSMULTIPLYPROCESS_H
#define ITKFILTERSMULTIPLYPROCESS_H

#include "itkFiltersProcessBase.h"
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersMultiplyProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersMultiplyProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersMultiplyProcess(void);
    virtual ~itkFiltersMultiplyProcess(void);
    
    virtual QString description ( void ) const;

    static bool registered ( void );
    
public slots:

    void setInput ( dtkAbstractData *data );
    void setParameter ( double  data, int channel );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    itkFiltersMultiplyProcessPrivate *d;
};


dtkAbstractProcess * createitkFiltersMultiplyProcess(void);

#endif // ITKFILTERSMULTIPLYPROCESS_H
