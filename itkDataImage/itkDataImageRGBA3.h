#ifndef _itk_DataImageRBGA3_h_
#define _itk_DataImageRBGA3_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageRGBA3 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageRGBA3(void);
    ~itkDataImageRGBA3(void);

    static bool registered(void);
};

#endif
