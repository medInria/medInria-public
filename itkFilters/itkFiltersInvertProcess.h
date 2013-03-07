#ifndef ITKFILTERSINVERTPROCESS_H
#define ITKFILTERSINVERTPROCESS_H

#include "itkFiltersProcessBase.h"
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersInvertProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersInvertProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersInvertProcess(itkFiltersInvertProcess * parent = 0);
    itkFiltersInvertProcess(const itkFiltersInvertProcess& other);
    virtual ~itkFiltersInvertProcess(void);
    
    itkFiltersInvertProcess& operator = (const itkFiltersInvertProcess& other);
    
    virtual QString description ( void ) const;

    static bool registered ( void );
    
public slots:

    void setInput ( dtkAbstractData *data );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersInvertProcess)
};


dtkAbstractProcess * createitkFiltersInvertProcess(void);

#endif // ITKFILTERSINVERTPROCESS_H
