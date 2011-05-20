#ifndef _itk_DataImageLong3_h_
#define _itk_DataImageLong3_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageLong3 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageLong3(void);
    ~itkDataImageLong3(void);

    static bool registered(void);
};

#endif
