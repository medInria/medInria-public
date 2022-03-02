#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore/QObject>

#include <medAbstractDatabaseImporter.h>
#include <medCoreLegacyExport.h>
#include <medDataIndex.h>

class medAbstractData;

/**
* @brief Reads files and adds them to the medNonPersistentDatabase.
*
* This class inherits from medJobItemL and is meant to be run by the medJobManagerL.
*
*/
class MEDCORELEGACY_EXPORT medDataPacsImporter : public medAbstractDatabaseImporter
{
    Q_OBJECT

public:
    /**
    * @brief Constructor.
    *
    * @param file the file or directory to be read.
    * @param callerUuid The string representation of a unique identifier. The caller will react to link the final signal with this id to know whether it should react to it or not.
    */
    medDataPacsImporter(const QString &file, const QUuid &uuid);
    ~medDataPacsImporter();

public:
    QString getPatientID(QString patientName, QString birthDate);

    medDataIndex populateDatabaseAndGenerateThumbnails(medAbstractData *medData, QString pathToStoreThumbnails);

    void setNumberOfFilesInDirectory(int num) override;

    QString ensureUniqueSeriesName(const QString &studyInstanceUID,
                                   const QString &seriesInstanceUID,
                                   const QString &seriesName);
};
