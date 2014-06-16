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

#include <itkDataSHImagePluginExport.h>
#include <itkImageIOBase.h>

class ITKDATASHIMAGEPLUGIN_EXPORT itkDataSHImageReaderBase : public dtkAbstractDataReader
{
    Q_OBJECT

public:
             itkDataSHImageReaderBase();
    virtual ~itkDataSHImageReaderBase();

    virtual QStringList handled() const;

    static QStringList s_handled();

public slots:

    /** if number of components of itk vector image is 15 or 28 returns true*/
    virtual bool canRead (const QString &path);
    virtual bool canRead (const QStringList &paths);

    /** it reads the information of vector itkDataSHImageFloat3 or itkDataSHImageDoublet3*/
    virtual bool readInformation (const QString &path);
    virtual bool readInformation (const QStringList &paths);

    /** where the itk::VectorImage<double/float, 3> is read*/
    virtual bool read (const QString &path);
    virtual bool read (const QStringList &paths);

protected:
    itk::ImageIOBase::Pointer io;
};
