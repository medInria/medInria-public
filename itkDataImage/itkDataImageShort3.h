#ifndef _itk_DataImageShort3_h_
#define _itk_DataImageShort3_h_

#include "itkDataImageBase.h"

#include "itkDataImagePluginExport.h"

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageShort3 : public itkDataImageBase
{
    Q_OBJECT
    
public:
    itkDataImageShort3(void);
    ~itkDataImageShort3(void);

    static bool registered(void);
};

#endif
