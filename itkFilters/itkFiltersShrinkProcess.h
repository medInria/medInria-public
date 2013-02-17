#ifndef ITKFILTERSSHRINKPROCESS_H
#define ITKFILTERSSHRINKPROCESS_H

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersShrinkProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersShrinkProcess : public dtkAbstractProcess
{
    Q_OBJECT
    
public:
    itkFiltersShrinkProcess(void);
    virtual ~itkFiltersShrinkProcess(void);
    
    virtual QString description ( void ) const;

    static bool registered ( void );

    void emitProgress(int progress);
    
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
