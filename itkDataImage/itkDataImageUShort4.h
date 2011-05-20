#ifndef _itk_DataImageUShort4_h_
#define _itk_DataImageUShort4_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageUShort4 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageUShort4(void);
    ~itkDataImageUShort4(void);

    static bool registered(void);
};

#endif
