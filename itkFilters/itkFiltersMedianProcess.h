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
    itkFiltersMedianProcess(itkFiltersMedianProcess * parent = 0);
    itkFiltersMedianProcess(const itkFiltersMedianProcess& other);
    virtual ~itkFiltersMedianProcess(void);

    static bool registered ( void );
    
public slots:

    int update ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersMedianProcess)
};


dtkAbstractProcess * createitkFiltersMedianProcess(void);


#endif // ITKFILTERSMEDIANPROCESS_H
