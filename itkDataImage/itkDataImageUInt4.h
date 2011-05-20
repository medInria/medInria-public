#ifndef _itk_DataImageUInt4_h_
#define _itk_DataImageUInt4_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageUInt4 : public itkDataImageBase
{
    Q_OBJECT

public:
    itkDataImageUInt4(void);
    ~itkDataImageUInt4(void);

    static bool registered(void);
};

#endif
