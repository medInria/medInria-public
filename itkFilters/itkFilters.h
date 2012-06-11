#ifndef ITKFILTERS_H
#define ITKFILTERS_H

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"
#include "itkCommand.h"

class itkFiltersPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFilters : public dtkAbstractProcess
{
    Q_OBJECT

public:
  
  enum FILTER
  {
    ADD, SUBTRACT, MULTIPLY, DIVIDE, GAUSSIAN, NORMALIZE, MEDIAN, INVERT, SHRINK, INTENSITY
  };
  
    itkFilters ( void );
    virtual ~itkFilters ( void );

    virtual QString description ( void ) const;

    static bool registered ( void );

    void emitProgress(int progress);
    
public slots:

    void setInput ( dtkAbstractData *data );
    void setParameter ( double  data, int channel );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    itkFiltersPrivate *d;
};

dtkAbstractProcess *createitkFilters ( void );
#endif // ITKFILTERS_H
