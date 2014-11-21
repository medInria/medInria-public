/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractDataReader.h>

#include <medTtkTensorImageExport.h>
#include <itkImageIOBase.h>

class medTtkTensorImagePlugin_EXPORT medTtkTensorImageDataReaderBase : public dtkAbstractDataReader
{
    Q_OBJECT

public:
             medTtkTensorImageDataReaderBase();
    virtual ~medTtkTensorImageDataReaderBase();

    virtual QStringList handled() const;

    static QStringList s_handled();

public slots:
    virtual bool canRead (const QString &path);
    virtual bool canRead (const QStringList &paths);
    
    virtual bool readInformation (const QString &path);
    virtual bool readInformation (const QStringList &paths);
    
    virtual bool read (const QString &path);
    virtual bool read (const QStringList &paths);

protected:
    itk::ImageIOBase::Pointer io;
};


