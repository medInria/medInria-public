/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medDataPacsController.h>
#include <medDatabaseNonPersistentItem.h>
#include <medDatabaseNonPersistentItem_p.h>
#include <medDataPacsImporter.h>
#include <medDataManager.h>
#include <medGlobalDefs.h>
#include <medMetaDataKeys.h>
#include <QString>

medDataPacsImporter::medDataPacsImporter(const QString &file, const QUuid &uuid)
    : medAbstractDatabaseImporter(file, uuid, true)
{
    qDebug() << "medDataPacsImporter created with uuid:" << uuid;
}

//-----------------------------------------------------------------------------------------------------------

medDataPacsImporter::~medDataPacsImporter()
{
}

//-----------------------------------------------------------------------------------------------------------
/**
* Retrieves patientID. Checks if patient is already in the database
* if so, reurns is Id, otherwise creates a new guid
*/
QString medDataPacsImporter::getPatientID(QString patientName, QString birthDate)
{
    QList<medDatabaseNonPersistentItem *> items = medDataPacsController::instance().items();

    bool patientExists = false;
    QString patientID;

    for (medDatabaseNonPersistentItem *item : items)
    {
        if (item->name() == patientName && item->birthdate() == birthDate)
        {
            patientExists = true;
            patientID = item->patientId();
            break;
        }
    }

    if (!patientExists)
    {
        patientID = QUuid::createUuid().toString().replace("{", "").replace("}", "");
    }

    return patientID;
}

//-----------------------------------------------------------------------------------------------------------
/**
* Populates database tables and generates thumbnails.
* @param medData - a @medAbstractData object created from the original image
* @param pathToStoreThumbnails - path where the thumbnails will be stored
* @return medDataIndex the new medDataIndex associated with this imported series.
**/
medDataIndex medDataPacsImporter::populateDatabaseAndGenerateThumbnails(medAbstractData *data, QString pathToStoreThumbnails)
{
    medDataIndex retIndex;
    Q_UNUSED(pathToStoreThumbnails);
    medDataPacsController &controller = medDataPacsController::instance();

    QString patientId = medMetaDataKeys::PatientID.getFirstValue(data);
    QString studyInstanceUID = medMetaDataKeys::StudyInstanceUID.getFirstValue(data);
    QString studyId = medMetaDataKeys::StudyID.getFirstValue(data);

    medDatabaseNonPersistentItem *studyItem = controller.getStudyItem(patientId, studyInstanceUID);
    if (studyItem)
    {
        medAbstractData *medData = studyItem->data();
        medData->setMetaData(medMetaDataKeys::StudyID.key(), QStringList() << studyId);
        studyItem->setStudyId(studyId);
    }

    QString seriesId = medMetaDataKeys::SeriesID.getFirstValue(data);
    QString seriesInstanceUID = medMetaDataKeys::SeriesInstanceUID.getFirstValue(data);
    QString orientation = medMetaDataKeys::Orientation.getFirstValue(data);
    QString seriesNumber = medMetaDataKeys::SeriesNumber.getFirstValue(data);
    QString sequenceName = medMetaDataKeys::SequenceName.getFirstValue(data);
    QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(data);
    QString rows = medMetaDataKeys::Rows.getFirstValue(data);
    QString columns = medMetaDataKeys::Columns.getFirstValue(data);

    QFileInfo info(file());
    medDatabaseNonPersistentItem *seriesItem = controller.getSeriesItem(patientId, studyInstanceUID, seriesInstanceUID);
    if (seriesItem)
    {
        QString patientName = medMetaDataKeys::PatientName.getFirstValue(data);
        QString birthdate = medMetaDataKeys::BirthDate.getFirstValue(data);
        QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(data);
        QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(data);

        if (seriesItem->name() == patientName && seriesItem->patientId() == patientId && seriesItem->birthdate() == birthdate && seriesItem->studyUid() == studyInstanceUID && seriesItem->seriesUid() == seriesInstanceUID)
        {
            if (seriesItem->data())
            {
                // seriesItem has already a medAbstractData";
                // we have to check others attributes";
                QList<medDataIndex> seriesIndexes = controller.series(medDataIndex::makeStudyIndex(controller.dataSourceId(), seriesItem->index().patientId(), seriesItem->index().studyId()));
                QList<medDatabaseNonPersistentItem *> items = controller.items();
                bool seriesFound = false;
                int numberOfSeries = 0;
                for (medDatabaseNonPersistentItem *item : items)
                {
                    medDataIndex id = item->index();
                    if (seriesIndexes.contains(id))
                    {
                        if (item->orientation() == orientation &&
                            item->seriesNumber() == seriesNumber &&
                            item->sequenceName() == sequenceName &&
                            item->sliceThickness() == sliceThickness &&
                            item->rows() == rows &&
                            item->columns() == columns)
                        {
                            numberOfSeries++;
                            seriesFound = true;
                            retIndex = id;
                        }
                        else
                        {
                            numberOfSeries++;
                        }
                    }
                }
                if (!seriesFound)
                {
                    // we have to create a new one";
                    medDataIndex newIndex = medDataIndex::makeSeriesIndex(controller.dataSourceId(), seriesItem->index().patientId(), seriesItem->index().studyId(), controller.seriesId(true));
                    medDatabaseNonPersistentItem *newItem = new medDatabaseNonPersistentItem;
                    newItem->setName(patientName);
                    newItem->setPatientId(patientId);
                    newItem->setBirthdate(birthdate);
                    newItem->setStudyName(studyName);
                    newItem->setSeriesName(seriesName);
                    newItem->setIndex(newIndex);
                    newItem->setSeriesUid(seriesInstanceUID);
                    newItem->setStudyUid(studyInstanceUID);
                    newItem->setSeriesId(seriesId);
                    newItem->setStudyId(studyId);
                    newItem->setFile(file());
                    newItem->setThumb(data->generateThumbnail(med::defaultThumbnailSize));
                    newItem->setData(data);

                    newItem->setOrientation(orientation);
                    newItem->setSeriesNumber(seriesNumber);
                    newItem->setSequenceName(sequenceName);
                    newItem->setSliceThickness(sliceThickness);
                    newItem->setRows(rows);
                    newItem->setColumns(columns);

                    controller.insert(newIndex, newItem);
                    retIndex = newIndex;
                }
            }
            else
            {
                seriesItem->setSeriesId(seriesId);
                seriesItem->setStudyId(studyId);
                seriesItem->setFile(file());
                seriesItem->setThumb(data->generateThumbnail(med::defaultThumbnailSize));
                seriesItem->setData(data);

                seriesItem->setOrientation(orientation);
                seriesItem->setSeriesNumber(seriesNumber);
                seriesItem->setSequenceName(sequenceName);
                seriesItem->setSliceThickness(sliceThickness);
                seriesItem->setRows(rows);
                seriesItem->setColumns(columns);
                retIndex = seriesItem->index();
            }
        }
    }
    return retIndex;
}

/**
* Finds if parameter @seriesName is already being used in the database
* if is not, it returns @seriesName unchanged
* otherwise, it returns an unused new series name (created by adding a suffix)
* @param seriesName - the series name
* @return newSeriesName - a new, unused, series name
**/
QString medDataPacsImporter::ensureUniqueSeriesName(const QString seriesName, const QString studyId)
{
    QStringList seriesNames = medDataPacsController::instance().series(seriesName, studyId);

    QString originalSeriesName = seriesName;
    QString newSeriesName = seriesName;

    int suffix = 0;
    while (seriesNames.contains(newSeriesName))
    {
        // it exist
        suffix++;
        newSeriesName = originalSeriesName + "_" + QString::number(suffix);
    }

    return newSeriesName;
}

void medDataPacsImporter::setNumberOfFilesInDirectory(int num)
{
    medDataPacsController::instance().setNumberOfFilesInDir(num);
}
