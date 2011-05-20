#ifndef _itk_DataImageShort4_h_
#define _itk_DataImageShort4_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageShort4 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageShort4(void);
    ~itkDataImageShort4(void);

    static bool registered(void);
};

#endif
