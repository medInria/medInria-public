#ifndef _itk_DataImageUChar3_h_
#define _itk_DataImageUChar3_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageUChar3 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageUChar3(void);
    ~itkDataImageUChar3(void);

    static bool registered(void);
};

#endif
