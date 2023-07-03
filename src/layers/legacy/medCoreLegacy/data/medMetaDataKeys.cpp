/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMetaDataKeys.h>

#include <medCoreLegacyExport.h>

#define CHAPTER_NAME     "chapterName"
#define CHAPTER_KEYS     "keys"
#define KEY_NAME         "name"
#define KEY_LABEL        "label"
#define KEY_TAG          "tag"
#define KEY_MEDKEY_PIVOT "medKey"
#define KEY_TYPE         "type"

#include <QDebug>


namespace medMetaDataKeys
{
    MEDCORELEGACY_EXPORT Key::Registery Key::registery;

    /** Define the actual keys to use */

    MEDCORELEGACY_EXPORT const Key TransferSyntaxUID("TransferSyntaxUID");
    MEDCORELEGACY_EXPORT const Key ContainsBasicInfo("ContainsBasicInfo");

    // PATIENT
    MEDCORELEGACY_EXPORT const Key PatientID("PatientID");
    MEDCORELEGACY_EXPORT const Key PatientName("PatientName", "Patient Name");
    MEDCORELEGACY_EXPORT const Key Age("Age", "Age"); // could be "00" format, or for instance "00Y"
    MEDCORELEGACY_EXPORT const Key BirthDate("BirthDate", "Birth Date"/*, QVariant::Date*/);
    MEDCORELEGACY_EXPORT const Key Gender("Gender", "Gender", QVariant::Char);
    MEDCORELEGACY_EXPORT const Key Description("Description"); // Used to add the app name and version

    // STUDY
    MEDCORELEGACY_EXPORT const Key StudyID("StudyID", "Study ID");
    MEDCORELEGACY_EXPORT const Key StudyInstanceUID("StudyInstanceUID", "Study Instance UID");
    MEDCORELEGACY_EXPORT const Key StudyDescription("StudyDescription", "Study Description");
    MEDCORELEGACY_EXPORT const Key Institution("Institution");
    MEDCORELEGACY_EXPORT const Key Referee("Referee");
    MEDCORELEGACY_EXPORT const Key StudyDate("StudyDate", "Study Date"/*, QVariant::Date*/);
    MEDCORELEGACY_EXPORT const Key StudyTime("StudyTime", "Study Time"/*, QVariant::Date*/);

    // SERIES
    MEDCORELEGACY_EXPORT const Key SeriesID("SeriesID", "Series ID");
    MEDCORELEGACY_EXPORT const Key SeriesInstanceUID("SeriesInstanceUID", "Series Instance UID");
    MEDCORELEGACY_EXPORT const Key SeriesNumber("SeriesNumber", "Series Number");
    MEDCORELEGACY_EXPORT const Key Modality("Modality");
    MEDCORELEGACY_EXPORT const Key Performer("Performer");
    MEDCORELEGACY_EXPORT const Key Report("Report");
    MEDCORELEGACY_EXPORT const Key Protocol("Protocol");
    MEDCORELEGACY_EXPORT const Key SeriesDescription("SeriesDescription", "Series Description");
    MEDCORELEGACY_EXPORT const Key SeriesDate("SeriesDate", "Series Date"/*, QVariant::Date*/);
    MEDCORELEGACY_EXPORT const Key SeriesTime("SeriesTime", "Series Time"/*, QVariant::Date*/);
    MEDCORELEGACY_EXPORT const Key SeriesThumbnail("SeriesThumbnail", "Series Thumbnail");

    // EQUIPEMENT
    MEDCORELEGACY_EXPORT const Key Manufacturer("Manufacturer", "Manufacturer");

    // IMAGE
    MEDCORELEGACY_EXPORT const Key SOPInstanceUID("SOPInstanceUID", "SOP Instance UID");
    MEDCORELEGACY_EXPORT const Key Columns("Columns","Columns",QVariant::UInt);
    MEDCORELEGACY_EXPORT const Key Rows("Rows","Rows",QVariant::UInt);
    MEDCORELEGACY_EXPORT const Key Dimensions("Dimensions","Dimensions",QVariant::Int);
    MEDCORELEGACY_EXPORT const Key NumberOfDimensions("NumberOfDimensions", "Number Of Dimensions");
    MEDCORELEGACY_EXPORT const Key Orientation("Orientation");
    MEDCORELEGACY_EXPORT const Key Origin("Origin");
    MEDCORELEGACY_EXPORT const Key SliceThickness("SliceThickness", "Slice Thickness");
    MEDCORELEGACY_EXPORT const Key ImportationDate("ImportationDate", "Importation Date");
    MEDCORELEGACY_EXPORT const Key AcquisitionDate("AcquisitionDate", "Acquisition Date");
    MEDCORELEGACY_EXPORT const Key AcquisitionTime("AcquisitionTime", "Acquisition Time");
    MEDCORELEGACY_EXPORT const Key Comments("Comments");
    MEDCORELEGACY_EXPORT const Key FilePaths("FilePaths", "File Paths");
    MEDCORELEGACY_EXPORT const Key Status("Status");
    MEDCORELEGACY_EXPORT const Key SequenceName("SequenceName", "Sequence Name");
    MEDCORELEGACY_EXPORT const Key Size("Size","Size",QVariant::UInt, false);
    MEDCORELEGACY_EXPORT const Key VolumeUID("VolumeUID", "Volume UID");
    MEDCORELEGACY_EXPORT const Key Spacing("Spacing");
    MEDCORELEGACY_EXPORT const Key XSpacing("XSpacing", "X Spacing");
    MEDCORELEGACY_EXPORT const Key YSpacing("YSpacing", "Y Spacing");
    MEDCORELEGACY_EXPORT const Key ZSpacing("ZSpacing", "Z Spacing");
    MEDCORELEGACY_EXPORT const Key NumberOfComponents("NumberOfComponents", "Number Of Components");
    MEDCORELEGACY_EXPORT const Key ComponentType("ComponentType", "Component Type");
    MEDCORELEGACY_EXPORT const Key PixelType("PixelType", "Pixel Type");
    MEDCORELEGACY_EXPORT const Key medDataType("medDataType", "Dtk Data Type");
    MEDCORELEGACY_EXPORT const Key PreferredDataReader("PreferredDataReader", "Preferred Data Reader");
    MEDCORELEGACY_EXPORT const Key ImageID("ImageID");
    MEDCORELEGACY_EXPORT const Key ThumbnailPath("ThumbnailPath", "Thumbnail Path");
    MEDCORELEGACY_EXPORT const Key AcquisitionNumber("AcquisitionNumber", "Acquisition Number");

    MEDCORELEGACY_EXPORT const Key PatientOrientation("PatientOrientation", "Patient Orientation");
    MEDCORELEGACY_EXPORT const Key PatientPosition("PatientPosition", "Patient Position");
    MEDCORELEGACY_EXPORT const Key ImageType("ImageType", "Image Type");

    // Frame of reference
    MEDCORELEGACY_EXPORT const Key FrameOfReferenceUID("FrameOfReferenceUID", "Frame of Reference UID");
    MEDCORELEGACY_EXPORT const Key PositionReferenceIndicator("PositionReferenceIndicator", "Position Reference Indicator");

    // MR Image
    MEDCORELEGACY_EXPORT const Key FlipAngle("FlipAngle", "Flip Angle");
    MEDCORELEGACY_EXPORT const Key EchoTime("EchoTime", "Echo Time");
    MEDCORELEGACY_EXPORT const Key RepetitionTime("RepetitionTime", "Repetition Time");

    // CT Image
    MEDCORELEGACY_EXPORT const Key KVP("KVP", "kVp", QVariant::UInt);





    bool medMetaDataKeys::fectChapterDirectory(QString path)
    {
        bool bRes = false;

        qDebug() << "medMetaDataKeys: Begin parsing metadata key directory to build a dictionary into : " << path;

        QDirIterator it(path, QStringList() << "*.json", QDir::Files);
        while (it.hasNext())
        {
            QString filePath = it.next();
            QFile file(filePath);
            file.open(QFile::ReadOnly);
            if (!loadChapter(file.readAll()))
            {
                qDebug() << "File : " << filePath << " is a malformed json key file";
            }

        }

        qDebug() << "medMetaDataKeys: Parsing ended into : " << path;

        return bRes;
    }

    bool medMetaDataKeys::loadChapter(QByteArray chapter)
    {
        bool bRes = false;

        QJsonDocument chapterDoc = QJsonDocument::fromJson(chapter);
        if (chapterDoc.isObject())
        {
            QJsonObject chapterObj = chapterDoc.object();
            if (chapterObj.contains(CHAPTER_NAME) && chapterObj[CHAPTER_NAME].isString() &&
                chapterObj.contains(CHAPTER_KEYS) && chapterObj[CHAPTER_KEYS].isArray())
            {
                QString chapterName = chapterObj[CHAPTER_NAME].toString().trimmed();
                QJsonArray keys = chapterObj[CHAPTER_KEYS].toArray();
                if (!chapterName.isEmpty() && !keys.isEmpty())
                {
                    for (auto entry : keys)
                    {
                        //Key2 keyOut;
                        if (entry.isObject())
                        {
                            QJsonObject key = entry.toObject();
                            //if (readKey(key, keyOut))
                            //{
                            //    bRes = true;
                            //}
                            //else
                            //{
                            //    qDebug() << "Unable to read a key in " << chapterName;
                            //}

                            //TODO register the key
                        }
                    }
                }
                else
                {
                    qDebug() << "Both, " << CHAPTER_KEYS << " and " << CHAPTER_NAME << " must be not empty values " << chapterName;
                }
            }
            else
            {
                qDebug() << "Both, " << CHAPTER_KEYS << " and " << CHAPTER_NAME << "are mandatories keys";
            }
        }

        return bRes;
    }


    bool medMetaDataKeys::readKey(QJsonObject keyAsJson, Key2 & key)
    {
        bool bRes = false;

        QString name;
        QString label; 
        QString tag; 
        QString medKey; 
        QVariant::Type type;

        if (keyAsJson.contains(KEY_NAME) && keyAsJson[KEY_NAME].isString())
        {
            name = keyAsJson[KEY_NAME].toString();
            bRes = !name.trimmed().isEmpty();

            if (bRes)
            {
                if (keyAsJson.contains(KEY_LABEL) && keyAsJson[KEY_LABEL].isString())
                {
                    label = keyAsJson[KEY_LABEL].toString().trimmed();
                }
                if (keyAsJson.contains(KEY_TAG) && keyAsJson[KEY_TAG].isString())
                {
                    tag = keyAsJson[KEY_TAG].toString().trimmed();
                }
                if (keyAsJson.contains(KEY_MEDKEY_PIVOT) && keyAsJson[KEY_MEDKEY_PIVOT].isString())
                {
                    medKey = keyAsJson[KEY_MEDKEY_PIVOT].toString().trimmed();
                }
                if (keyAsJson.contains(KEY_TYPE) && keyAsJson[KEY_TYPE].isString())
                {
                    type = QVariant::nameToType(keyAsJson[KEY_TYPE].toString().toUtf8().data());
                }

                //key = Key2(name, label, tag, medKey, type);
            }
            else
            {
                qDebug() << "Key as invalid format. Name must be a non empty string.";
            }

        }
        else
        {
            qDebug() << "Key as invalid format. Name is mandatory.";
        }

        return bRes;
    }
};

