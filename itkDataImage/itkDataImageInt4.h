#ifndef _itk_DataImageInt4_h_
#define _itk_DataImageInt4_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageInt4 : public itkDataImageBase
{
    Q_OBJECT

public:
    itkDataImageInt4(void);
    ~itkDataImageInt4(void);

    static bool registered(void);
};

#endif
