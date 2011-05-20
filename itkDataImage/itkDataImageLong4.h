#ifndef _itk_DataImageLong4_h_
#define _itk_DataImageLong4_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageLong4 : public itkDataImageBase
{
    Q_OBJECT

public:
    itkDataImageLong4(void);
    ~itkDataImageLong4(void);

    static bool registered(void);
};

#endif
