#pragma once

#include <itkDataImagePluginExport.h>

#include "itkDicomDataImageWriter.h"

class ITKDATAIMAGEPLUGIN_EXPORT DicomRtImageWriter : public itkDicomDataImageWriter
{
public:
    DicomRtImageWriter();

    QString identifier()  const override;
    QString description() const override;
    QStringList handled() const override;
    QStringList supportedFileExtensions() const override;

    static bool registered();

    static dtkAbstractDataWriter* create();

protected:
    void fillDictionaryFromMetaDataKey(itk::MetaDataDictionary &dictionary, bool &studyUIDExistance) override;
};
