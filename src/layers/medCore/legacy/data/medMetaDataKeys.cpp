/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMetaDataKeys.h>

namespace medMetaDataKeys
{
    MEDCORE_EXPORT Key::Registery Key::registery;

    /** Define the actual keys to use */

    MEDCORE_EXPORT const Key TransferSyntaxUID("TransferSyntaxUID");
    MEDCORE_EXPORT const Key ContainsBasicInfo("ContainsBasicInfo");

    // PATIENT
    MEDCORE_EXPORT const Key PatientID("PatientID");
    MEDCORE_EXPORT const Key PatientName("PatientName", "Patient Name");
    MEDCORE_EXPORT const Key Age("Age", "Age"); // could be "00" format, or for instance "00Y"
    MEDCORE_EXPORT const Key BirthDate("BirthDate", "Birth Date"/*, QVariant::Date*/);
    MEDCORE_EXPORT const Key Gender("Gender", "Gender", QVariant::Char);
    MEDCORE_EXPORT const Key Description("Description"); // Used to add the app name and version

    // STUDY
    MEDCORE_EXPORT const Key StudyID("StudyID", "Study ID");
    MEDCORE_EXPORT const Key StudyInstanceUID("StudyInstanceUID", "Study Instance UID");
    MEDCORE_EXPORT const Key StudyDescription("StudyDescription", "Study Description");
    MEDCORE_EXPORT const Key Institution("Institution");
    MEDCORE_EXPORT const Key Referee("Referee");
    MEDCORE_EXPORT const Key StudyDate("StudyDate", "Study Date"/*, QVariant::Date*/);
    MEDCORE_EXPORT const Key StudyTime("StudyTime", "Study Time"/*, QVariant::Date*/);

    // SERIES
    MEDCORE_EXPORT const Key SeriesID("SeriesID", "Series ID");
    MEDCORE_EXPORT const Key SeriesInstanceUID("SeriesInstanceUID", "Series Instance UID");
    MEDCORE_EXPORT const Key SeriesNumber("SeriesNumber", "Series Number");
    MEDCORE_EXPORT const Key Modality("Modality");
    MEDCORE_EXPORT const Key Performer("Performer");
    MEDCORE_EXPORT const Key Report("Report");
    MEDCORE_EXPORT const Key Protocol("Protocol");
    MEDCORE_EXPORT const Key SeriesDescription("SeriesDescription", "Series Description");
    MEDCORE_EXPORT const Key SeriesDate("SeriesDate", "Series Date"/*, QVariant::Date*/);
    MEDCORE_EXPORT const Key SeriesTime("SeriesTime", "Series Time"/*, QVariant::Date*/);
    MEDCORE_EXPORT const Key SeriesThumbnail("SeriesThumbnail", "Series Thumbnail");

    // EQUIPEMENT
    MEDCORE_EXPORT const Key Manufacturer("Manufacturer", "Manufacturer");

    // IMAGE
    MEDCORE_EXPORT const Key SOPInstanceUID("SOPInstanceUID", "SOP Instance UID");
    MEDCORE_EXPORT const Key Columns("Columns","Columns",QVariant::UInt);
    MEDCORE_EXPORT const Key Rows("Rows","Rows",QVariant::UInt);
    MEDCORE_EXPORT const Key Dimensions("Dimensions","Dimensions",QVariant::Int);
    MEDCORE_EXPORT const Key NumberOfDimensions("NumberOfDimensions", "Number Of Dimensions");
    MEDCORE_EXPORT const Key Orientation("Orientation");
    MEDCORE_EXPORT const Key Origin("Origin");
    MEDCORE_EXPORT const Key SliceThickness("SliceThickness", "Slice Thickness");
    MEDCORE_EXPORT const Key ImportationDate("ImportationDate", "Importation Date");
    MEDCORE_EXPORT const Key AcquisitionDate("AcquisitionDate", "Acquisition Date");
    MEDCORE_EXPORT const Key AcquisitionTime("AcquisitionTime", "Acquisition Time");
    MEDCORE_EXPORT const Key Comments("Comments");
    MEDCORE_EXPORT const Key FilePaths("FilePaths", "File Paths");
    MEDCORE_EXPORT const Key Status("Status");
    MEDCORE_EXPORT const Key SequenceName("SequenceName", "Sequence Name");
    MEDCORE_EXPORT const Key Size("Size","Size",QVariant::UInt, false);
    MEDCORE_EXPORT const Key VolumeUID("VolumeUID", "Volume UID");
    MEDCORE_EXPORT const Key Spacing("Spacing");
    MEDCORE_EXPORT const Key XSpacing("XSpacing", "X Spacing");
    MEDCORE_EXPORT const Key YSpacing("YSpacing", "Y Spacing");
    MEDCORE_EXPORT const Key ZSpacing("ZSpacing", "Z Spacing");
    MEDCORE_EXPORT const Key NumberOfComponents("NumberOfComponents", "Number Of Components");
    MEDCORE_EXPORT const Key ComponentType("ComponentType", "Component Type");
    MEDCORE_EXPORT const Key PixelType("PixelType", "Pixel Type");
    MEDCORE_EXPORT const Key medDataType("medDataType", "Dtk Data Type");
    MEDCORE_EXPORT const Key PreferredDataReader("PreferredDataReader", "Preferred Data Reader");
    MEDCORE_EXPORT const Key ImageID("ImageID");
    MEDCORE_EXPORT const Key ThumbnailPath("ThumbnailPath", "Thumbnail Path");
    MEDCORE_EXPORT const Key AcquisitionNumber("AcquisitionNumber", "Acquisition Number");

    MEDCORE_EXPORT const Key PatientOrientation("PatientOrientation", "Patient Orientation");
    MEDCORE_EXPORT const Key PatientPosition("PatientPosition", "Patient Position");
    MEDCORE_EXPORT const Key ImageType("ImageType", "Image Type");

    // Frame of reference
    MEDCORE_EXPORT const Key FrameOfReferenceUID("FrameOfReferenceUID", "Frame of Reference UID");
    MEDCORE_EXPORT const Key PositionReferenceIndicator("PositionReferenceIndicator", "Position Reference Indicator");

    // MR Image
    MEDCORE_EXPORT const Key FlipAngle("FlipAngle", "Flip Angle");
    MEDCORE_EXPORT const Key EchoTime("EchoTime", "Echo Time");
    MEDCORE_EXPORT const Key RepetitionTime("RepetitionTime", "Repetition Time");

    // CT Image

    // EXPORT EXTRA DATA TO ATTACHED FILE
    // MEDCORE_EXPORT const Key Toolbox("Toolbox", "Toolbox used to process image");
    // MEDCORE_EXPORT const Key OriginalDataUID("OriginalDataUID", "UID of original data used to generate new data");
    // MEDCORE_EXPORT const Key OriginalDataDesc("OriginalDataDesc", "Description of original data used to generate new data");
    // MEDCORE_EXPORT const Key FileMetadataPath("FileMetadataPath", "Path to attached metadata file");
    MEDCORE_EXPORT const Key KVP("KVP", "kVp", QVariant::UInt);
};

