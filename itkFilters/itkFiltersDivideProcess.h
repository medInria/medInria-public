#ifndef ITKFILTERSDIVIDEPROCESS_H
#define ITKFILTERSDIVIDEPROCESS_H

#include "itkFiltersProcessBase.h"
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersDivideProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersDivideProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersDivideProcess(itkFiltersDivideProcess * parent = 0);
    itkFiltersDivideProcess(const itkFiltersDivideProcess& other);
    virtual ~itkFiltersDivideProcess(void);
    
    itkFiltersDivideProcess& operator = (const itkFiltersDivideProcess& other);
    
    virtual QString description ( void ) const;

    static bool registered ( void );
    
public slots:

    void setInput ( dtkAbstractData *data );
    void setParameter ( double  data, int channel );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersDivideProcess)
};


dtkAbstractProcess * createitkFiltersDivideProcess(void);

#endif // ITKFILTERSDIVIDEPROCESS_H
