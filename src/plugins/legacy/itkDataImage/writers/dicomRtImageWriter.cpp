#include "dicomRtImageWriter.h"

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>

#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkMetaDataObject.h>

#include <string>

static QString s_identifier()
{
    return "DicomRtImageWriter";
}

static QStringList s_handled() {
    return QStringList()  << "itkDataImageChar3"
                          << "itkDataImageUChar3"
                          << "itkDataImageShort3"
                          << "itkDataImageUShort3"
                          << "itkDataImageInt3"
                          << "itkDataImageUInt3"
                          << "itkDataImageLong3"
                          << "itkDataImageULong3"
                          << "itkDataImageFloat3"
                          << "itkDataImageDouble3";
}

DicomRtImageWriter::DicomRtImageWriter()
    : itkDicomDataImageWriter()
{
}

QStringList DicomRtImageWriter::handled() const
{
    return s_handled();
}

QStringList DicomRtImageWriter::supportedFileExtensions() const
{
    return QStringList() << ".dcm";
}

bool DicomRtImageWriter::registered()
{
    return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString DicomRtImageWriter::identifier() const
{
    return s_identifier();
}

QString DicomRtImageWriter::description() const
{
    return "Dicom Rt image exporter for Varian Eclipse";
}

void DicomRtImageWriter::fillDictionaryFromMetaDataKey(itk::MetaDataDictionary& dictionary, bool& studyUIDExistance)
{
    itkDicomDataImageWriter::fillDictionaryFromMetaDataKey(dictionary, studyUIDExistance);

    if (this->data())
    {
        // Eclipse requires some tags to be present and/or to have specific values.
        // For example, the patient' sex cannot be anonymized.

        // First, check if those required tags exist in the metaDataList
        QStringList metaDataList = this->data()->metaDataList();

        // Image Type
        if (!metaDataList.contains(medMetaDataKeys::ImageType.key()))
        {
            // our data did not have an Image Type, create a default one:
            // DERIVED: pixel values have been derived from one or more
            // images (this is true for all masks).
            // SECONDARY: image created after the initial patient examination.
            QString imageType = "DERIVED\\SECONDARY";
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0008", imageType.toStdString());
        }

        // Patient's gender
        if (metaDataList.contains(medMetaDataKeys::Gender.key()))
        {
            // check the value
            QString gender = this->data()->metaDataValues(medMetaDataKeys::Gender.key()).first();
            if (gender != "M" && gender != "F" && gender != "O")
            {
                // Anonymized or invalid gender, use default
                itk::EncapsulateMetaData<std::string>(dictionary, "0010|0040", "O");
            }
        }
        else
        {
            // Patient's gender was not exported... Export 'default'
            itk::EncapsulateMetaData<std::string>(dictionary, "0010|0040", "O");
        }
    }
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter* DicomRtImageWriter::create()
{
    return new DicomRtImageWriter;
}

