#ifndef _itk_DataImageVector3_h_
#define _itk_DataImageVector3_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageVector3 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageVector3(void);
    ~itkDataImageVector3(void);

    static bool registered(void);
};

#endif
