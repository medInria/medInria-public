#ifndef ITKFILTERSNORMALIZEPROCESS_H
#define ITKFILTERSNORMALIZEPROCESS_H

#include "itkFiltersProcessBase.h"
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersNormalizeProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersNormalizeProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersNormalizeProcess(void);
    virtual ~itkFiltersNormalizeProcess(void);
    
    virtual QString description ( void ) const;

    static bool registered ( void );
    
public slots:

    void setInput ( dtkAbstractData *data );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    itkFiltersNormalizeProcessPrivate *d;
};


dtkAbstractProcess * createitkFiltersNormalizeProcess(void);


#endif // ITKFILTERSNORMALIZEPROCESS_H
