#ifndef _itk_DataImageChar4_h_
#define _itk_DataImageChar4_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageChar4 : public itkDataImageBase
{
    Q_OBJECT

public:
    itkDataImageChar4(void);
    ~itkDataImageChar4(void);

    static bool registered(void);
};

#endif
