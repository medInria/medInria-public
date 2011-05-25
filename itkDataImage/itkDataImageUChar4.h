#ifndef _itk_DataImageUChar4_h_
#define _itk_DataImageUChar4_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageUChar4 : public itkDataImageBase
{
    Q_OBJECT

public:
    itkDataImageUChar4(void);
    ~itkDataImageUChar4(void);

    static bool registered(void);
};

#endif
