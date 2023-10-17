/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 All rights reserved.
 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <itkDataImageReaderBase.h>
#include "itkINRDataImageReaderPluginExport.h"

class ITKINRDATAIMAGEREADERPLUGIN_EXPORT itkINRDataImageReader : public itkDataImageReaderBase
{
public:
    itkINRDataImageReader();
    ~itkINRDataImageReader() = default;

    QString description() const;
    QStringList handled() const;
    static QStringList s_handled();
    static bool registered();
};

dtkAbstractDataReader *createItkINRDataImageReader();
