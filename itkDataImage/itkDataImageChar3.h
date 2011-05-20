#ifndef _itk_DataImageChar3_h_
#define _itk_DataImageChar3_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageChar3 : public itkDataImageBase
{
    Q_OBJECT

public:
    itkDataImageChar3(void);
    ~itkDataImageChar3(void);

    static bool registered(void);
};

#endif
