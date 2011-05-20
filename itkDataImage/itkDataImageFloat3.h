#ifndef _itk_DataImageFloat3_h_
#define _itk_DataImageFloat3_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageFloat3 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageFloat3(void);
    ~itkDataImageFloat3(void);

    static bool registered(void);
};

#endif
