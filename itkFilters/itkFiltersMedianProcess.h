#ifndef ITKFILTERSMEDIANPROCESS_H
#define ITKFILTERSMEDIANPROCESS_H

#include "itkFiltersProcessBase.h"
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersMedianProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersMedianProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersMedianProcess(void);
    virtual ~itkFiltersMedianProcess(void);
    
    virtual QString description ( void ) const;

    static bool registered ( void );
    
public slots:

    void setInput ( dtkAbstractData *data );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    itkFiltersMedianProcessPrivate *d;
};


dtkAbstractProcess * createitkFiltersMedianProcess(void);


#endif // ITKFILTERSMEDIANPROCESS_H
