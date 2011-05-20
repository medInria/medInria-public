#ifndef _itk_DataImageInt3_h_
#define _itk_DataImageInt3_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageInt3 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageInt3(void);
    ~itkDataImageInt3(void);

    static bool registered(void);
};

#endif
