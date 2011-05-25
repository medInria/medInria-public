#ifndef _itk_DataImageULong4_h_
#define _itk_DataImageULong4_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageULong4 : public itkDataImageBase
{
    Q_OBJECT

public:
    itkDataImageULong4(void);
    ~itkDataImageULong4(void);

    static bool registered(void);
};

#endif
