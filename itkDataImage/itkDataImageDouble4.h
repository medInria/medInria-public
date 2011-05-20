#ifndef _itk_DataImageDouble4_h_
#define _itk_DataImageDouble4_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageDouble4 : public itkDataImageBase
{
    Q_OBJECT

public:
    itkDataImageDouble4(void);
    ~itkDataImageDouble4(void);

    static bool registered(void);
};

#endif
