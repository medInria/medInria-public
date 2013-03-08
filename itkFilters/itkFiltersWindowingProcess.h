#ifndef ITKFILTERSWINDOWINGPROCESS_H
#define ITKFILTERSWINDOWINGPROCESS_H

#include "itkFiltersProcessBase.h"
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersWindowingProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersWindowingProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersWindowingProcess(itkFiltersWindowingProcess * parent = 0);
    itkFiltersWindowingProcess(const itkFiltersWindowingProcess& other);
    virtual ~itkFiltersWindowingProcess(void);
    
    itkFiltersWindowingProcess& operator = (const itkFiltersWindowingProcess& other);
    
    virtual QString description ( void ) const;

    static bool registered ( void );
    
public slots:

    void setInput ( dtkAbstractData *data );
    void setParameter ( double  data, int channel );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersWindowingProcess)
};


dtkAbstractProcess * createitkFiltersWindowingProcess(void);

#endif // ITKFILTERSWINDOWINGPROCESS_H
