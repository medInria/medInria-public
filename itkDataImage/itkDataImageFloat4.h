#ifndef _itk_DataImageFloat4_h_
#define _itk_DataImageFloat4_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageFloat4 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageFloat4(void);
    ~itkDataImageFloat4(void);

    static bool registered(void);
};

#endif
