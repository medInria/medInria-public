#ifndef _itk_DataImage_h_
#define _itk_DataImage_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageRGB3 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageRGB3(void);
    ~itkDataImageRGB3(void);

    static bool registered(void);
};

#endif
