/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medMetaDataKeys.h"

namespace medMetaDataKeys {

    MEDCORE_EXPORT Key::Registery Key::registery;

    /** Define the actual keys to use */

    MEDCORE_EXPORT const Key TransferSyntaxUID("TransferSyntaxUID");

    // PATIENT
    MEDCORE_EXPORT const Key PatientID("PatientID");
    MEDCORE_EXPORT const Key PatientName("PatientName");
    MEDCORE_EXPORT const Key Age("Age");
    MEDCORE_EXPORT const Key BirthDate("BirthDate");
    MEDCORE_EXPORT const Key Gender("Gender");
    MEDCORE_EXPORT const Key Description("Description"); //what?

    // STUDY
    MEDCORE_EXPORT const Key StudyID("StudyID");
    MEDCORE_EXPORT const Key StudyDicomID("StudyDicomID");
    MEDCORE_EXPORT const Key StudyDescription("StudyDescription");
    MEDCORE_EXPORT const Key Institution("Institution");
    MEDCORE_EXPORT const Key Referee("Referee");
    MEDCORE_EXPORT const Key StudyDate("StudyDate");
    MEDCORE_EXPORT const Key StudyTime("StudyTime");

    // SERIES
    MEDCORE_EXPORT const Key SeriesID("SeriesID");
    MEDCORE_EXPORT const Key SeriesDicomID("SeriesDicomID");
    MEDCORE_EXPORT const Key SeriesNumber("SeriesNumber");
    MEDCORE_EXPORT const Key Modality("Modality");
    MEDCORE_EXPORT const Key Performer("Performer");
    MEDCORE_EXPORT const Key Report("Report");
    MEDCORE_EXPORT const Key Protocol("Protocol");
    MEDCORE_EXPORT const Key SeriesDescription("SeriesDescription");
    MEDCORE_EXPORT const Key SeriesDate("SeriesDate");
    MEDCORE_EXPORT const Key SeriesTime("SeriesTime");
    MEDCORE_EXPORT const Key SeriesThumbnail("SeriesThumbnail");

    // IMAGE

    MEDCORE_EXPORT const Key SOPInstanceUID("SOPInstanceUID");
    MEDCORE_EXPORT const Key Columns("Columns");
    MEDCORE_EXPORT const Key Rows("Rows");
    MEDCORE_EXPORT const Key Dimensions("Dimensions");
    MEDCORE_EXPORT const Key NumberOfDimensions("NumberOfDimensions");
    MEDCORE_EXPORT const Key Orientation("Orientation");
    MEDCORE_EXPORT const Key Origin("Origin");
    MEDCORE_EXPORT const Key SliceThickness("SliceThickness");
    MEDCORE_EXPORT const Key ImportationDate("ImportationDate");
    MEDCORE_EXPORT const Key AcquisitionDate("AcquisitionDate");
    MEDCORE_EXPORT const Key AcquisitionTime("AcquisitionTime");
    MEDCORE_EXPORT const Key Comments("Comments");
    MEDCORE_EXPORT const Key FilePaths("FilePaths");
    MEDCORE_EXPORT const Key Status("Status");
    MEDCORE_EXPORT const Key SequenceName("SequenceName");
    MEDCORE_EXPORT const Key Size("Size");
    MEDCORE_EXPORT const Key VolumeUID("VolumeUID");
    MEDCORE_EXPORT const Key Spacing("Spacing");
    MEDCORE_EXPORT const Key XSpacing("XSpacing");
    MEDCORE_EXPORT const Key YSpacing("YSpacing");
    MEDCORE_EXPORT const Key ZSpacing("ZSpacing");
    MEDCORE_EXPORT const Key NumberOfComponents("NumberOfComponents");
    MEDCORE_EXPORT const Key ComponentType("ComponentType");
    MEDCORE_EXPORT const Key PixelType("PixelType");
    MEDCORE_EXPORT const Key DtkDataType("DtkDataType");
    MEDCORE_EXPORT const Key PreferredDataReader("PreferredDataReader");
    MEDCORE_EXPORT const Key ImageID("ImageID");
    MEDCORE_EXPORT const Key ThumbnailPath("ThumbnailPath");
};
