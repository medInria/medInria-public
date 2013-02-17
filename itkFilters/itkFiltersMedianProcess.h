#ifndef ITKFILTERSMEDIANPROCESS_H
#define ITKFILTERSMEDIANPROCESS_H

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersMedianProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersMedianProcess : public dtkAbstractProcess
{
    Q_OBJECT
    
public:
    itkFiltersMedianProcess(void);
    virtual ~itkFiltersMedianProcess(void);
    
    virtual QString description ( void ) const;

    static bool registered ( void );

    void emitProgress(int progress);
    
public slots:

    void setInput ( dtkAbstractData *data );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    itkFiltersMedianProcessPrivate *d;
};


dtkAbstractProcess * createitkFiltersMedianProcess(void);


#endif // ITKFILTERSMEDIANPROCESS_H
