#ifndef _itk_DataImageUInt3_h_
#define _itk_DataImageUInt3_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageUInt3 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageUInt3(void);
    ~itkDataImageUInt3(void);

    static bool registered(void);
};

#endif
