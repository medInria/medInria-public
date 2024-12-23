#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDataReader.h>

#include <medImageIOExport.h>

#include <itkImageIOBase.h>

class MEDIMAGEIO_EXPORT itkDataImageReaderBase: public medAbstractDataReader
{
    Q_OBJECT

public:

    itkDataImageReaderBase();
    virtual ~itkDataImageReaderBase();

    virtual QStringList handled() const = 0;

public slots:

    virtual bool canRead (const QString& path);
    virtual bool canRead (const QStringList& paths);

    virtual bool readInformation (const QString& path);
    virtual bool readInformation (const QStringList& paths);

    virtual bool read (const QString& path);
    virtual bool read (const QStringList& paths);

    virtual void setProgress (int value);

protected:

    itk::ImageIOBase::Pointer io;
    QHash<QString, QString> itkKeyToMedKey;

    QString convertItkKeyToMedKey(std::string& keyToConvert);

private:
    template <unsigned DIM,typename T>
    bool read_image(const QString& path,const char* type);
    bool extractMetaData();
};
