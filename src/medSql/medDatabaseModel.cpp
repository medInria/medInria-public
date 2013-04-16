/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtGui>
#include <QtCore>
#include <QtSql>

#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>

#include <medDatabaseController.h>
#include <medDatabaseItem.h>
#include <medDatabaseModel.h>
#include <medDatabaseNonPersistentController.h>

#include <medAbstractDbController.h>
#include <medDbControllerFactory.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medAbstractDatabaseItem.h>

// /////////////////////////////////////////////////////////////////
// medDatabaseModelPrivate
// /////////////////////////////////////////////////////////////////

class medDatabaseModelPrivate
{
public:
    medAbstractDatabaseItem *item(const QModelIndex& index) const;

public:
    bool justBringStudies;

    medAbstractDatabaseItem *root;

    QList<QVariant> ptAttributes;  // Attributes displayed on Patient rows
    QList<QVariant> stAttributes;  // Attributes displayed on studies rows
    QList<QVariant> seAttributes;  // Attributes displayed on series rows.

    QList<QVariant> ptDefaultData;
    QList<QVariant> stDefaultData;
    QList<QVariant> seDefaultData;

    QList<QVariant> data;
    QList<QString> columnNames;

    enum { DataCount = 19 };
};

medAbstractDatabaseItem *medDatabaseModelPrivate::item(const QModelIndex& index) const
{
    if (index.isValid()) {
        medAbstractDatabaseItem *item = static_cast<medAbstractDatabaseItem *>(index.internalPointer());
        if (item)
            return item;
    }

    return root;
}

// /////////////////////////////////////////////////////////////////
// medDatabaseModel
// /////////////////////////////////////////////////////////////////

medDatabaseModel::medDatabaseModel(QObject *parent, bool justBringStudies) : QAbstractItemModel(parent), d(new medDatabaseModelPrivate)
{
    d->justBringStudies = justBringStudies;

    QString NulString;
    const int dataCount = d->DataCount;

    d->ptAttributes = QList<QVariant>();
#if QT_VERSION > 0x0406FF
    d->ptAttributes.reserve(dataCount);
#endif
    for (int i(0); i<dataCount; ++i)
        d->ptAttributes.append(NulString);

    d->ptAttributes[0] = medMetaDataKeys::PatientName.key();
    d->ptAttributes[5] = medMetaDataKeys::BirthDate.key();
    d->ptAttributes[6] = medMetaDataKeys::Gender.key();

    d->stAttributes = QList<QVariant>();
#if QT_VERSION > 0x0406FF
    d->stAttributes.reserve(dataCount);
#endif
    for (int i(0); i<dataCount; ++i)
        d->stAttributes.append(NulString);

    d->stAttributes[1] = medMetaDataKeys::StudyDescription.key();

    d->seAttributes = QList<QVariant>();
#if QT_VERSION > 0x0406FF
    d->seAttributes.reserve(dataCount);
#endif
    for (int i(0); i<dataCount; ++i)
        d->seAttributes.append(NulString);
    d->seAttributes[1] = medMetaDataKeys::StudyDescription.key();
    d->seAttributes[2] = medMetaDataKeys::SeriesDescription.key();
    d->seAttributes[3] = medMetaDataKeys::Size.key();
    d->seAttributes[4] = medMetaDataKeys::Age.key();
    d->seAttributes[7] = medMetaDataKeys::SeriesDescription.key();
    d->seAttributes[8] = medMetaDataKeys::Modality.key();
    d->seAttributes[12] = medMetaDataKeys::AcquisitionDate.key();
    d->seAttributes[13] = medMetaDataKeys::ImportationDate.key();
    d->seAttributes[15] = medMetaDataKeys::Referee.key();
    d->seAttributes[16] = medMetaDataKeys::Performer.key();
    d->seAttributes[17] = medMetaDataKeys::Institution.key();
    d->seAttributes[18] = medMetaDataKeys::Report.key();

    d->data = QList<QVariant>();
#if QT_VERSION > 0x0406FF
    d->data.reserve(dataCount);
#endif
    for (int i(0); i<dataCount; ++i)
        d->data.append(NulString);

    d->ptDefaultData =  d->data;
    d->ptDefaultData[0] = tr("[No Patient Name]");

    d->stDefaultData =  d->data;
    d->stDefaultData[1] = tr("[No Study Name]");

    d->seDefaultData =  d->data;
    d->seDefaultData[1] = tr("[No Study Name]");
    d->seDefaultData[2] = tr("[No Series Name]");

    d->root = new medDatabaseItem(medDataIndex(), d->data, d->data);

    populate(d->root);

    connect(medDataManager::instance(), SIGNAL(dataAdded(medDataIndex)), this, SLOT(repopulate()));
    connect(medDataManager::instance(), SIGNAL(dataRemoved(medDataIndex)), this, SLOT(repopulate()));
    connect(medDbControllerFactory::instance(), SIGNAL(dbControllerRegistered(const QString&)), this, SLOT(repopulate()));
}

medDatabaseModel::~medDatabaseModel(void)
{
    delete d->root;
    delete d;

    d = NULL;
}

int medDatabaseModel::rowCount(const QModelIndex& parent) const
{
    medAbstractDatabaseItem *parentItem;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = d->root;
    else
        parentItem = static_cast<medAbstractDatabaseItem *>(parent.internalPointer());

    return parentItem->childCount();
}

int medDatabaseModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<medAbstractDatabaseItem *>(parent.internalPointer())->columnCount();
    else
        return d->root->columnCount();
}

int medDatabaseModel::columnIndex(const QString& title) const
{
    for(int i = 0 ; i < d->root->columnCount() ; i++)
    if(d->root->data(i).toString() == title)
        return i;

    return -1;
}

QVariant medDatabaseModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    medAbstractDatabaseItem *item = static_cast<medAbstractDatabaseItem *>(index.internalPointer());

    return item->data(index.column());
}

QVariant medDatabaseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return this->columnNames().at(section);

    return QVariant();
}

// This is really a shocking search. A hash table is to be used instead !!!!

QModelIndex medDatabaseModel::indexForPatient(int id) const
{
    for(int i = 0; i < this->rowCount(); i++) {
        QModelIndex index = this->index(i, 0);
        if(medAbstractDatabaseItem *item = static_cast<medAbstractDatabaseItem *>(index.internalPointer()))
            if(item->data(0).toInt() == id)
                return index;
    }

    return QModelIndex();
}

// This is really a shocking search. A hash table is to be used instead !!!!

QModelIndex medDatabaseModel::indexForStudy(int id) const
{
    for(int i = 0; i < this->rowCount(); i++) {
        QModelIndex patientIndex = this->index(i, 0);
        if(medAbstractDatabaseItem *patientItem = static_cast<medAbstractDatabaseItem *>(patientIndex.internalPointer()))
            for(int j = 0; j < patientItem->childCount(); j++) {
                QModelIndex index = this->index(j, 0, patientIndex);
                if(medAbstractDatabaseItem *item = static_cast<medAbstractDatabaseItem *>(index.internalPointer()))
                    if(item->data(0).toInt() == id)
                        return index;
            }
    }

    return QModelIndex();
}

// This is really a shocking search. A hash table is to be used instead !!!!

QModelIndex medDatabaseModel::indexForSeries(int id) const
{
    for(int i = 0; i < this->rowCount(); i++) {
        QModelIndex patientIndex = this->index(i, 0);
        if(medAbstractDatabaseItem *patientItem = static_cast<medAbstractDatabaseItem *>(patientIndex.internalPointer()))
            for(int j = 0; j < patientItem->childCount(); j++) {
                QModelIndex studyIndex = this->index(j, 0, patientIndex);
                if(medAbstractDatabaseItem *studyItem = static_cast<medAbstractDatabaseItem *>(studyIndex.internalPointer()))
                    for(int k = 0; k < studyItem->childCount(); k++) {
                        QModelIndex index = this->index(k, 0, studyIndex);
                        if(medAbstractDatabaseItem *item = static_cast<medAbstractDatabaseItem *>(index.internalPointer()))
                            if(item->data(0).toInt() == id)
                                return index;
                    }
            }
    }

    return QModelIndex();
}

// This is really a shocking search. A hash table is to be used instead !!!!

QModelIndex medDatabaseModel::indexForImage(int id) const
{
    for(int i = 0; i < this->rowCount(); i++) {
        QModelIndex patientIndex = this->index(i, 0);
        if(medAbstractDatabaseItem *patientItem = static_cast<medAbstractDatabaseItem *>(patientIndex.internalPointer()))
            for(int j = 0; j < patientItem->childCount(); j++) {
                QModelIndex studyIndex = this->index(j, 0, patientIndex);
                if(medAbstractDatabaseItem *studyItem = static_cast<medAbstractDatabaseItem *>(studyIndex.internalPointer()))
                    for(int k = 0; k < studyItem->childCount(); k++) {
                        QModelIndex seriesIndex = this->index(k, 0, studyIndex);
                        if(medAbstractDatabaseItem *seriesItem = static_cast<medAbstractDatabaseItem *>(seriesIndex.internalPointer()))
                            for(int l = 0; l < seriesItem->childCount(); l++) {
                                QModelIndex index = this->index(l, 0, seriesIndex);
                                if(medAbstractDatabaseItem *item = static_cast<medAbstractDatabaseItem *>(index.internalPointer()))
                                    if(item->data(0).toInt() == id)
                                        return index;
                            }
                    }
            }
    }

    return QModelIndex();
}

QModelIndex medDatabaseModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    medAbstractDatabaseItem *parentItem;
    if (!parent.isValid())
        parentItem = d->root;
    else
        parentItem = static_cast<medAbstractDatabaseItem *>(parent.internalPointer());

    medAbstractDatabaseItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex medDatabaseModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    medAbstractDatabaseItem *child = static_cast<medAbstractDatabaseItem *>(index.internalPointer());
    medAbstractDatabaseItem *parent = child->parent();

    if (parent == d->root)
        return QModelIndex();

    return createIndex(parent->row(), 0, parent);
}

//! Get item flags for a model index.
/*!
 * Used by other components to obtain information about each item
 * provided by the model.
 *
 * \param index The index of the item for which flags are required.
 * \return A combination of flags.
 */

Qt::ItemFlags medDatabaseModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsSelectable | /*Qt::ItemIsEditable |*/ Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled;
}

//! Sets the role data for the item at index to value. Returns true if
//! successful, otherwise returns false.
/*!
 * This method is used to modify the item of data associated with a
 * specified model index.
 *
 * \param index The index of the data item to be modified.
 * \param value The value to be set for the data item.
 * \param role  The role for which the value is to be changed.
 *
 * \return true if the update is successful, false otherwise.
 */
bool medDatabaseModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole)
        return false;

    if(index.column() == 0)
        return false;

    medAbstractDatabaseItem *item = d->item(index);

    bool result = item->setData(index.column(), value);

    if(result) {
        medDataIndex dataIndex = item->dataIndex();
        medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource( dataIndex.dataSourceId() );
        QString attribute = item->attribute(index.column()).toString();
        QString value     = item->value(index.column()).toString();
        bool success = dbc->setMetaData( dataIndex, attribute, value );
        if ( !success ) {
            dtkDebug() << "Could not set data for index " << dataIndex.asString();
        }
    }

    return result;
}

bool medDatabaseModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = d->root->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

bool medDatabaseModel::insertColumns(int position, int columns, const QModelIndex& parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = d->root->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool medDatabaseModel::removeColumns(int position, int columns, const QModelIndex& parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = d->root->removeColumns(position, columns);
    endRemoveColumns();

    if (d->root->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool medDatabaseModel::insertRows(int position, int rows, const QModelIndex& parent)
{
    medAbstractDatabaseItem *parentItem = d->item(parent);

    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(medDataIndex(), position, rows, d->root->columnCount());
    endInsertRows();

    return success;
}

bool medDatabaseModel::removeRows(int position, int rows, const QModelIndex& parent)
{
    medAbstractDatabaseItem *parentItem = d->item(parent);

    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

// /////////////////////////////////////////////////////////////////
// Drag and drop support
// /////////////////////////////////////////////////////////////////

QStringList medDatabaseModel::mimeTypes(void) const
{
    return QAbstractItemModel::mimeTypes() << "text/uri-list";
}

Qt::DropActions medDatabaseModel::supportedDropActions(void) const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

bool medDatabaseModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("text/uri-list"))
        return false;

    if (!data->hasUrls())
        return false;

    if (column > 0)
        return false;

    for (int i = 0; i < data->urls().size(); ++i)
        medDatabaseController::instance()->import(data->urls().at(i).path());

    return true;
}

// /////////////////////////////////////////////////////////////////
// Initialization
// /////////////////////////////////////////////////////////////////

void medDatabaseModel::repopulate(void)
{
    beginRemoveRows(QModelIndex(),0,rowCount());
    if (rowCount() > 0)
        this->removeRows(0, this->rowCount(QModelIndex()), QModelIndex());
    endRemoveRows();

    beginInsertRows(QModelIndex(),0,0);
    populate(d->root);
    endInsertRows();

    reset();
}

//! Model population.
/*!
 *  This method fills the model in with the data. The actual data is
 *  contained within an medDatabaseItem and the later is accessed from
 *  an index using the QModelIndex::internalPointer() method.
 *
 * \param root The root item of the model.
 */

void medDatabaseModel::populate(medAbstractDatabaseItem *root)
{
    typedef QList<int> IntList;
    typedef QList<medDataIndex> IndexList;
    medDataManager * dataManager = medDataManager::instance();
    IntList dataSources = dataManager->dataSourceIds();
    {
        // Initial Rearrangement, so that non-Persistent data sources are at the end.
        IntList npDataSources;
        for (IntList::iterator dataSourceIt(dataSources.begin()); dataSourceIt!= dataSources.end(); ) {
            medAbstractDbController * dbc = dataManager->controllerForDataSource(*dataSourceIt);
            if ( !dbc ) {
                dataSourceIt = dataSources.erase(dataSourceIt);
            } else if ( dbc->isPersistent() ) {
                ++dataSourceIt;
            } else {
                npDataSources.push_back(*dataSourceIt);
                dataSourceIt = dataSources.erase(dataSourceIt);
            }
        }
        dataSources << npDataSources;
    }

    foreach( const int dataSourceId, dataSources ) {

        medAbstractDbController * dbc = dataManager->controllerForDataSource(dataSourceId);
        
        IndexList patientsForSource = dbc->patients();

        // Iterate over patientIds for this data source
        foreach( const medDataIndex& patient, patientsForSource ) {
            QList<QVariant> ptData = d->ptDefaultData;
            for (int i(0); i<d->DataCount; ++i) {
                QVariant attribute = d->ptAttributes[i];
                if ( !attribute.isNull() ) {
                    QString value =  dbc->metaData(patient, attribute.toString() );
                    if ( !value.isEmpty() )
                        ptData[i] = value;
                }
            }
            medAbstractDatabaseItem *ptItem = new medDatabaseItem(patient, d->ptAttributes, ptData, root);

            IndexList studiesForSource = dbc->studies(patient);

            // Iterate over studyIds for this patient
            foreach( const medDataIndex& study, studiesForSource ) {

                if (d->justBringStudies)
                {
                      QList<QVariant> stData = d->stDefaultData;
                      for (int i(0); i<d->DataCount; ++i) {
                          QVariant attribute = d->stAttributes[i];
                          if ( !attribute.isNull() ) {
                              QString value =  dbc->metaData(study, attribute.toString() );
                              if ( !value.isEmpty() )
                                  stData[i] = value;
                          }
                      }
                      medAbstractDatabaseItem *stItem = new medDatabaseItem(study, d->stAttributes, stData, ptItem);

                      ptItem->append(stItem);

                }
                else
                {
                    IndexList seriesForSource = dbc->series(study);

                        // Iterate over series for this study
                        foreach( const medDataIndex& serie, seriesForSource ) {

                            QList<QVariant> seData = d->seDefaultData;
                            for (int i(0); i<d->DataCount; ++i) {
                                QVariant attribute = d->seAttributes[i];
                                if ( !attribute.isNull() ) {
                                    QString value =  dbc->metaData(serie, attribute.toString() );
                                    if ( !value.isEmpty() )
                                        seData[i] = value;
                                }
                            }
                            medAbstractDatabaseItem *seItem = new medDatabaseItem(serie, d->seAttributes, seData, ptItem);

                            ptItem->append(seItem);
                        } // foreach series
                }

            } // foreach study
            // ptItem->append(stItem);
            root->append(ptItem);
        } // foreach patient
    } // foreach dataSource
}

QStringList medDatabaseModel::columnNames() const
{
    if ( d->columnNames.isEmpty() ) {
        QStringList ret;
#if QT_VERSION > 0x0406FF
        ret.reserve( d->DataCount );
#endif
        for (int i(0); i<d->DataCount; ++i)
            ret.append(QString());

        ret[0] = tr("Patient name");
        ret[1] = tr("Study name");
        ret[2] = tr("Series name");
        ret[3] = tr("Slice Count");
        ret[4] = tr("Age");
        ret[5] = tr("Date of birth");
        ret[6] = tr("Gender");
        ret[7] = tr("Description");
        ret[8] = tr("Modality");
        ret[9] = tr("Protocol");
        ret[10] = tr("Comments");
        ret[11] = tr("Status");
        ret[12] = tr("Date acquired");
        ret[13] = tr("Date imported");
        ret[14] = tr("Last opened");
        ret[15] = tr("Referee");
        ret[16] = tr("Performer");
        ret[17] = tr("Institution");
        ret[18] = tr("Report");
        d->columnNames = ret;
    }
    return d->columnNames;
}
