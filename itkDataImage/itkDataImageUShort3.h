#ifndef _itk_DataImageUShort3_h_
#define _itk_DataImageUShort3_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageUShort3 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageUShort3(void);
    ~itkDataImageUShort3(void);

    static bool registered(void);
};

#endif
