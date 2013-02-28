#ifndef ITKFILTERSSHRINKPROCESS_H
#define ITKFILTERSSHRINKPROCESS_H

#include "itkFiltersProcessBase.h"
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersShrinkProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersShrinkProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersShrinkProcess(void);
    virtual ~itkFiltersShrinkProcess(void);
    
    virtual QString description ( void ) const;

    static bool registered ( void );
    
public slots:

    void setInput ( dtkAbstractData *data );
    void setParameter ( double  data, int channel );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    itkFiltersShrinkProcessPrivate *d;
};


dtkAbstractProcess * createitkFiltersShrinkProcess(void);

#endif // ITKFILTERSSHRINKPROCESS_H
