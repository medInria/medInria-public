#ifndef _itk_DataImageULong3_h_
#define _itk_DataImageULong3_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageULong3 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageULong3(void);
    ~itkDataImageULong3(void);

    static bool registered(void);
};

#endif
