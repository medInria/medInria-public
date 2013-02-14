#ifndef ITKFILTERSDIVIDEPROCESS_H
#define ITKFILTERSDIVIDEPROCESS_H

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersDivideProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersDivideProcess : public dtkAbstractProcess
{
    Q_OBJECT
    
public:
    itkFiltersDivideProcess(void);
    virtual ~itkFiltersDivideProcess(void);
    
    virtual QString description ( void ) const;

    static bool registered ( void );

    void emitProgress(int progress);
    
public slots:

    void setInput ( dtkAbstractData *data );
    void setParameter ( double  data, int channel );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    itkFiltersDivideProcessPrivate *d;
};


dtkAbstractProcess * createitkFiltersDivideProcess(void);

#endif // ITKFILTERSDIVIDEPROCESS_H
