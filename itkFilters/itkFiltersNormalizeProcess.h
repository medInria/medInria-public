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
    itkFiltersNormalizeProcess(itkFiltersNormalizeProcess * parent = 0);
    itkFiltersNormalizeProcess(const itkFiltersNormalizeProcess& other);
    virtual ~itkFiltersNormalizeProcess(void);

    static bool registered ( void );
    
public slots:

    int update ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersNormalizeProcess)
};

dtkAbstractProcess * createitkFiltersNormalizeProcess(void);


#endif // ITKFILTERSNORMALIZEPROCESS_H
