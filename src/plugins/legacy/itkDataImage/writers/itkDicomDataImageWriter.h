#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataImageWriterBase.h>
#include <itkDataImagePluginExport.h>

#include <itkGDCMImageIO.h>

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>


class ITKDATAIMAGEPLUGIN_EXPORT itkDicomDataImageWriter: public itkDataImageWriterBase
{
public:
    dtkSmartPointer<medAbstractData> dataCopy;

public:
    itkDicomDataImageWriter();
    virtual ~itkDicomDataImageWriter();

    virtual QString identifier()  const;
    virtual QString description() const;
    virtual QStringList handled() const;
    virtual QStringList supportedFileExtensions() const;

    static bool registered();

    static dtkAbstractDataWriter * create();

    QString sopClassUID(QString modality);

public slots:
    virtual bool write(const QString &path);

protected:
    virtual void fillDictionaryFromMetaDataKey(itk::MetaDataDictionary &dictionary, bool &studyUIDExistance);
    virtual void fillDictionaryWithModalityDependentData(itk::MetaDataDictionary& dictionary);

    template <class PixelType> bool writeDicom(const QString &path);
    template <class PixelType> void fillDictionaryWithSharedData(itk::MetaDataDictionary &dictionary, bool studyUIDExistance,
                                                                 itk::GDCMImageIO::Pointer gdcmIO, int &numberOfSlices);
    template <class PixelType> bool fillDictionaryAndWriteDicomSlice(itk::MetaDataDictionary &dictionary,  const QString &path,
                                                                     itk::GDCMImageIO::Pointer gdcmIO, int slice);
};
