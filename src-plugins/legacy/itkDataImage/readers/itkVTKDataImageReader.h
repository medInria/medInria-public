/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkDataImageReaderBase.h>

#include <itkDataImagePluginExport.h>

class ITKDATAIMAGEPLUGIN_EXPORT itkVTKDataImageReader: public itkDataImageReaderBase {
public:
    itkVTKDataImageReader();
    virtual ~itkVTKDataImageReader();

    virtual QString identifier() const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled();

    static bool registered();

private:

    static const char ID[];
};

dtkAbstractDataReader *createItkVTKDataImageReader();


