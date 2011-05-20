#ifndef _itk_DataImageDouble3_h_
#define _itk_DataImageDouble3_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageDouble3 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageDouble3(void);
    ~itkDataImageDouble3(void);

    static bool registered(void);
};

#endif
