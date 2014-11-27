/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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

    QList<bool> editFlags;

    QList<QVariant> data;
    QList<QString> columnNames;

    medDataIndex draggedDataIndex;

    QHash<medDataIndex, QModelIndex> medIndexMap;

    enum { DataCount = 12 };
};

medAbstractDatabaseItem *medDatabaseModelPrivate::item(const QModelIndex& index) const
{
    if (index.isValid())
    {
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
    d->draggedDataIndex = medDataIndex();

    QString NulString;
    const int dataCount = d->DataCount;

    d->ptAttributes = QList<QVariant>();
    d->stAttributes = QList<QVariant>();
    d->seAttributes = QList<QVariant>();
    d->editFlags = QList<bool>();
    d->data = QList<QVariant>();

#if QT_VERSION > 0x0406FF
    d->ptAttributes.reserve(dataCount);
    d->stAttributes.reserve(dataCount);
    d->seAttributes.reserve(dataCount);
    d->data.reserve(dataCount);
    d->editFlags.reserve(dataCount);
#endif
    for (int i(0); i<dataCount; ++i)
    {
        d->ptAttributes.append(NulString);
        d->stAttributes.append(NulString);
        d->seAttributes.append(NulString);
        d->data.append(".");
        //by default all fields are editable
        d->editFlags.append(true);
    }

    int i = 0;
    d->ptAttributes[i] = medMetaDataKeys::PatientName.key();
    d->stAttributes[i] = medMetaDataKeys::StudyDescription.key();
    d->seAttributes[i++] = medMetaDataKeys::SeriesDescription.key();

    d->ptAttributes[i++] = medMetaDataKeys::BirthDate.key();
    d->ptAttributes[i++] = medMetaDataKeys::Gender.key();

    // the number of slice is not supposed to be edited
    d->editFlags[i] = false;
    d->seAttributes[i++] = medMetaDataKeys::Size.key();
    d->seAttributes[i++] = medMetaDataKeys::Age.key();

    d->seAttributes[i++] = medMetaDataKeys::Modality.key();
    d->seAttributes[i++] = medMetaDataKeys::AcquisitionDate.key();
    d->seAttributes[i++] = medMetaDataKeys::ImportationDate.key();
    d->seAttributes[i++] = medMetaDataKeys::Referee.key();
    d->seAttributes[i++] = medMetaDataKeys::Performer.key();
    d->seAttributes[i++] = medMetaDataKeys::Institution.key();
    d->seAttributes[i++] = medMetaDataKeys::Report.key();


    d->ptDefaultData =  d->data;
    d->ptDefaultData[0] = tr("[No Patient Name]");

    d->stDefaultData =  d->data;

    d->seDefaultData =  d->data;


    d->root = new medDatabaseItem(medDataIndex(), d->data, d->data);

    this->populate(d->root);

    connect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)), this, SLOT(update(medDataIndex)), Qt::QueuedConnection);
    connect(medDataManager::instance(), SIGNAL(dataRemoved(medDataIndex)), this, SLOT(update(medDataIndex)), Qt::QueuedConnection);
    connect(medDataManager::instance(), SIGNAL(metadataModified(medDataIndex,QString,QString)), this, SLOT(update(medDataIndex)), Qt::QueuedConnection);
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

bool medDatabaseModel::hasChildren ( const QModelIndex & parent ) const
{
    return (rowCount(parent) > 0);
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

    if( role==Qt::TextAlignmentRole && index.column() > 0)
        return Qt::AlignHCenter;

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


QModelIndex medDatabaseModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    medAbstractDatabaseItem *parentItem;
    if (!parent.isValid())
    {
        parentItem = d->root;
    }
    else
        parentItem = static_cast<medAbstractDatabaseItem *>(parent.internalPointer());

    medAbstractDatabaseItem *childItem = parentItem->child(row);
    if (childItem)
    {
        QModelIndex newIndex = createIndex(row, column, childItem);

        // we only need index with column 0 in index map
        if(column==0)
        {
            d->medIndexMap[childItem->dataIndex()] = newIndex;
        }

        return newIndex;
    }
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

    QModelIndex newIndex =  createIndex(parent->row(), 0, parent);
    d->medIndexMap[parent->dataIndex()] = newIndex;

    return newIndex;

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
    medAbstractDatabaseItem *item = d->item(index);
    medDataIndex dataIndex= item->dataIndex();

    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if( dataIndex.isValidForSeries() )
    {
        flags = flags | Qt::ItemIsDragEnabled;

        if(!d->seAttributes[index.column()].toString().isEmpty() && d->editFlags[index.column()] == true)
        {
            flags = flags | Qt::ItemIsEditable;
        }
    }
    else if( dataIndex.isValidForStudy() )
    {
        flags = flags | Qt::ItemIsDragEnabled;

        if(!d->stAttributes[index.column()].toString().isEmpty() && d->editFlags[index.column()] == true)
        {
            flags = flags | Qt::ItemIsEditable;
        }
    }
    else if(dataIndex.isValidForPatient())
    {
        if(!d->ptAttributes[index.column()].toString().isEmpty() && d->editFlags[index.column()] == true)
        {
            flags = flags | Qt::ItemIsEditable;
        }
    }

    if( d->draggedDataIndex.isValid() )
    {
        //drops allowed only between the same datasources
        if( dataIndex.dataSourceId() == d->draggedDataIndex.dataSourceId() )
        {
            flags = flags | Qt::ItemIsDropEnabled;
        }
    }

    return flags;
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
    bool result = false;

    if (role != Qt::EditRole)
        return false;

    if (d->editFlags[index.column()] == false)
        return false;

    medAbstractDatabaseItem *item = d->item(index);
    medDataIndex dataIndex = item->dataIndex();
    QString attribute = item->attribute(index.column()).toString();

    //first, we try to set metadata
    result = medDataManager::instance()->setMetadata( dataIndex, attribute, value.toString() );

    if ( !result )
    {
        dtkDebug() << "Could not set data for index " << dataIndex.asString();
    }
    else
    {
        //and if we succeed, we try to update item
        result = item->setData(index.column(), value);

        if(result)
            emit dataChanged(index, index);
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
    return QAbstractItemModel::mimeTypes() << "med/index" << "med/DbItem";
}

Qt::DropActions medDatabaseModel::supportedDropActions(void) const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

QMimeData *medDatabaseModel::mimeData(const QModelIndexList &indexes) const
{
    medAbstractDatabaseItem *item = d->item(indexes[0]);

    if(item)
    {
        medDataIndex dataIndex = item->dataIndex();
        d->draggedDataIndex = dataIndex;
        return dataIndex.createMimeData();
    }
    else
    {
        d->draggedDataIndex = medDataIndex();
        return NULL;
    }
}

bool medDatabaseModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    Q_UNUSED(row);

    if (action == Qt::IgnoreAction)
        return true;

    // this  doesn't seem to be used anywhere
    if (data->hasFormat("text/uri-list")) {
        if (!data->hasUrls())
            return false;

        if (column > 0)
            return false;

        for (int i = 0; i < data->urls().size(); ++i)
            medDataManager::instance()->importPath(data->urls().at(i).path(),true);

        return true;
    } else if ( data->hasFormat("med/index") ) {
        medDataIndex destinationDataIndex = d->item(parent)->dataIndex();
        medDataIndex originDataIndex = medDataIndex::readMimeData(data);

        QList<medDataIndex> newIndexList;

        if( originDataIndex.isValidForSeries()) {
            if ( destinationDataIndex.isValidForSeries() || destinationDataIndex.isValidForStudy()) {
                newIndexList << medDataManager::instance()->moveSerie(originDataIndex, destinationDataIndex);
            }
        }
        else if( originDataIndex.isValidForStudy() && destinationDataIndex.isValidForPatient()) {
            newIndexList = medDataManager::instance()->moveStudy(originDataIndex, destinationDataIndex);
        }

        if( !newIndexList.isEmpty() && newIndexList[0].isValid()) {
            return true;
        }

        return false;
    }
    return false;
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

    IntList dataSources;
    dataSources << medDatabaseController::instance()->dataSourceId()
                << medDatabaseNonPersistentController::instance()->dataSourceId();

    foreach( const int dataSourceId, dataSources )
    {

        medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(dataSourceId);

        IndexList patientsForSource = dbc->patients();

        // Iterate over patientIds for this data source
        foreach( const medDataIndex& patient, patientsForSource )
        {
            QList<QVariant> ptData = d->ptDefaultData;
            for (int i(0); i<d->DataCount; ++i)
            {
                QVariant attribute = d->ptAttributes[i].toString();
                if ( !attribute.isNull() )
                {
                    QString value =  dbc->metaData(patient, attribute.toString() );
                    QVariant data = convertQStringToQVariant(attribute.toString(),value);
                    if ( data.isValid() )
                        ptData[i] = data;
                }
            }
            medAbstractDatabaseItem *ptItem = new medDatabaseItem(patient, d->ptAttributes, ptData, root);

            IndexList studiesForSource = dbc->studies(patient);

            // Iterate over studyIds for this patient
            foreach( const medDataIndex& study, studiesForSource )
            {
                QList<QVariant> stData = d->stDefaultData;
                for (int i(0); i<d->DataCount; ++i)
                {
                    QVariant attribute = d->stAttributes[i];
                    if ( !attribute.isNull() )
                    {
                        QString value =  dbc->metaData(study, attribute.toString() );
                        QVariant data = convertQStringToQVariant(attribute.toString(),value);
                        if ( data.isValid() )
                            stData[i] = data;
                    }
                }

                medAbstractDatabaseItem *stItem = new medDatabaseItem(study, d->stAttributes, stData, ptItem);
                ptItem->append(stItem);

                IndexList seriesForSource = dbc->series(study);

                // justBringStudies: not sure this is useful anymore
                if(!d->justBringStudies)
                {
                    // Iterate over series for this study
                    foreach( const medDataIndex& serie, seriesForSource )
                    {

                        QList<QVariant> seData = d->seDefaultData;
                        for (int i(0); i<d->DataCount; ++i)
                        {
                            QVariant attribute = d->seAttributes[i];
                            if ( !attribute.isNull() )
                            {
                                QString value =  dbc->metaData(serie, attribute.toString() );
                                QVariant data = convertQStringToQVariant(attribute.toString(),value);
                                if ( data.isValid() )
                                    seData[i] = data;
                            }
                        }
                        medAbstractDatabaseItem *seItem = new medDatabaseItem(serie, d->seAttributes, seData, stItem);

                        stItem->append(seItem);
                    } // foreach series
                }
            }

            root->append(ptItem);
        } // foreach patient
    } // foreach dataSource
}




void medDatabaseModel::update(const medDataIndex& dataIndex)
{
    if(dataIndex.isValidForSeries())
    {
        updateSerie(dataIndex);
    }
    else if(dataIndex.isValidForStudy())
    {
        updateStudy(dataIndex);
    }
    else if(dataIndex.isValidForPatient())
    {
        updatePatient(dataIndex);
    }
}

void medDatabaseModel::updateSerie(const medDataIndex& dataIndex)
{
    // different cases:
    //    - the serie is not present in the db, we have to remove it from the model
    //    - the serie is present but there is no item: we need to create one
    //    - the serie is present and there is an item: we need to update the data

    QModelIndex index = d->medIndexMap[dataIndex];
    medAbstractDatabaseItem *item = static_cast<medAbstractDatabaseItem *>(index.internalPointer());
    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(dataIndex.dataSourceId());

    if(!dbc->contains(dataIndex))
    {
        if(item)
        {
            // data is not present in the database anymore,
            QModelIndex parentIndex = index.parent();
            medDataIndex stDataIndex(dataIndex);
            stDataIndex.setSeriesId(-1);
            QModelIndex stIndex = d->medIndexMap.value(stDataIndex);

            medAbstractDatabaseItem *parent = item->parent();
            if(!parent)
            {
                qWarning() << "A problem occured while updating the serie " << dataIndex.asString();
            }
            else
            {
                emit layoutAboutToBeChanged();
                changePersistenIndexAndSubIndex(index);
                parent->removeChildren(/*index.row()*/parent->rowOf(item), 1);
                d->medIndexMap.remove(dataIndex);

                emit layoutChanged();
                emit dataChanged(parentIndex, parentIndex);
            }
        }

        return;
    }

    if(dataIndex.isValidForSeries())
    {
        QList<QVariant> seData = d->seDefaultData;
        for (int i(0); i<d->DataCount; ++i)
        {
            QVariant attribute = d->seAttributes[i];
            if ( !attribute.isNull() )
            {
                QString value =  dbc->metaData(dataIndex, attribute.toString() );
                QVariant data = convertQStringToQVariant(attribute.toString(),value);
                if ( data.isValid() )
                {
                    seData[i] = data;
                    if(item)
                        item->setData(i, data);
                }
            }
        }

        if(!item)
        {
            medDataIndex stDataIndex(dataIndex);
            stDataIndex.setSeriesId(-1);

            QModelIndex stIndex = d->medIndexMap.value(stDataIndex);

            medAbstractDatabaseItem *stItem = static_cast<medAbstractDatabaseItem *>(stIndex.internalPointer());

            //in some cases (when importing for example), a serie is being created while there is no study item)
            if(!stItem)
            {
                updateStudy(stDataIndex, false);
                stIndex = d->medIndexMap.value(stDataIndex);
                stItem = static_cast<medAbstractDatabaseItem *>(stIndex.internalPointer());
                if(!stItem)
                {
                    qWarning() << "A problem occured while updating the serie " << dataIndex.asString();
                }
            }

            //we must create a new item for the serie
            medAbstractDatabaseItem *seItem = new medDatabaseItem(dataIndex, d->seAttributes, seData, stItem);

            //and append it to the parent study
            emit layoutAboutToBeChanged();
            stItem->append(seItem);
            emit layoutChanged();

            //calling index() to update medIndexMap
            QModelIndex newIndex = this->index(stItem->childCount()-1,0,stIndex);

             emit dataChanged(newIndex.parent().parent(), newIndex.parent().parent());
             emit dataChanged(newIndex.parent(), newIndex.parent());
             emit dataChanged(newIndex, newIndex);
        }
    }

    return;
}

void medDatabaseModel::updateStudy(const medDataIndex& dataIndex, bool updateChildren)
{
    // different cases:
    //    - the study is not present in the db, we have to remove it from the model
    //    - the study is present but there is no item: we need to create one
    //    - the study is present and there is an item: we need to update the data and its series (in case of a move)
    //
    QModelIndex index = d->medIndexMap.value(dataIndex);
    medAbstractDatabaseItem *item = static_cast<medAbstractDatabaseItem *>(index.internalPointer());
    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(dataIndex.dataSourceId());

    if(!dbc->contains(dataIndex))
    {
        if(item)
        {
            QModelIndex parentIndex = index.parent();
            QList<medDataIndex> series = dbc->series(dataIndex);
            foreach(medDataIndex serie, series)
                updateSerie(serie);

            emit layoutAboutToBeChanged();

            changePersistenIndexAndSubIndex(index);

            medDataIndex ptDataIndex(dataIndex);
            ptDataIndex.setStudyId(-1);

            QModelIndex ptIndex = d->medIndexMap.value(ptDataIndex);
            medAbstractDatabaseItem *parent = item->parent();

            if(!parent)
            {
                qWarning() << "A problem occured while updating the serie " << dataIndex.asString();
            }
            else
            {
                parent->removeChildren(/*index.row()*/parent->rowOf(item), 1);
                d->medIndexMap.remove(dataIndex);
            }

            emit layoutChanged();

            emit dataChanged(parentIndex, parentIndex);
        }

        return;
    }

    if(dataIndex.isValidForStudy())
    {
        QList<QVariant> stData = d->stDefaultData;
        for (int i(0); i<d->DataCount; ++i)
        {
            QVariant attribute = d->stAttributes[i];
            if ( !attribute.isNull() )
            {
                QString value =  dbc->metaData(dataIndex, attribute.toString() );
                QVariant data = convertQStringToQVariant(attribute.toString(),value);
                if ( data.isValid() )
                {
                    stData[i] = data;
                    if(item)
                        item->setData(i, data);
                }
            }
        }

        if(!item)
        {
            // we must create a new item
            // and append it to the parent patientId

            medDataIndex ptDataIndex(dataIndex);
            ptDataIndex.setStudyId(-1);

            QModelIndex ptIndex = d->medIndexMap.value(ptDataIndex);

            medAbstractDatabaseItem *ptItem = static_cast<medAbstractDatabaseItem *>(ptIndex.internalPointer());

            //in some cases (when importing for example), a serie is being created while there is no study or patient item)
            if(!ptItem)
            {
                updatePatient(ptDataIndex, false);
                ptIndex = d->medIndexMap.value(ptDataIndex);
                ptItem = static_cast<medAbstractDatabaseItem *>(ptIndex.internalPointer());
                if(!ptItem)
                {
                    qWarning() << "A problem occured while updating the serie " << dataIndex.asString();
                }
            }
            if(ptItem)
            {
                medAbstractDatabaseItem *stItem = new medDatabaseItem(dataIndex, d->stAttributes, stData, ptItem);
                emit layoutAboutToBeChanged();
                ptItem->append(stItem);

                //calling index() to update medIndexMap
                QModelIndex newIndex = this->index(ptItem->childCount()-1,0,ptIndex);

                emit layoutChanged();

                emit dataChanged(newIndex.parent(), newIndex.parent());
                emit dataChanged(newIndex, newIndex);
            }
        }

        if(updateChildren)
        {
            QList<medDataIndex> series = dbc->series(dataIndex);
            foreach(medDataIndex serie, series)
                updateSerie(serie);
        }
    }

     return;
}

void medDatabaseModel::updatePatient(const medDataIndex& dataIndex, bool updateChildren)
{
    QModelIndex index = d->medIndexMap.value(dataIndex);
    medAbstractDatabaseItem *item = static_cast<medAbstractDatabaseItem *>(index.internalPointer());
    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(dataIndex.dataSourceId());

    if(!dbc->contains(dataIndex))
    {
        if(item)
        {
            medAbstractDatabaseItem *parent = item->parent();
            if(!parent)
            {
                qWarning() << "A problem occured while updating the serie " << dataIndex.asString();
            }
            else
            {
                // we can't do that, studies are already deleted at this stage
                // QList<medDataIndex> studies = dbc->studies(dataIndex);
                emit layoutAboutToBeChanged();

                changePersistenIndexAndSubIndex(index);

                foreach(medDataIndex tempIndex, d->medIndexMap.keys())
                {
                    if(medDataIndex::isMatch(dataIndex, tempIndex))
                        d->medIndexMap.remove(tempIndex);
                }
                item->removeChildren(0, item->childCount());

                // data is not present in the database anymore,
                // we need to update the parent to update its children

                item->parent()->removeChildren(index.row(), 1);
                d->medIndexMap.remove(dataIndex);

                emit layoutChanged();
            }
        }

        return;
    }

    if(dataIndex.isValidForPatient())
    {
        QList<QVariant> ptData = d->ptDefaultData;
        for (int i(0); i<d->DataCount; ++i)
        {
            QVariant attribute = d->ptAttributes[i].toString();
            if ( !attribute.isNull() )
            {
                QString value =  dbc->metaData(dataIndex, attribute.toString() );
                QVariant data = convertQStringToQVariant(attribute.toString(),value);
                if ( data.isValid() )
                {
                    ptData[i] = data;
                    if(item)
                        item->setData(i, data);
                }
            }
        }

        if(!item)
        {
            // we must create a new item
            // and append it to the parent patientId
            medAbstractDatabaseItem *ptItem = new medDatabaseItem(dataIndex, d->ptAttributes, ptData,  d->root);

            emit layoutAboutToBeChanged();
            d->root->append(ptItem);
            QModelIndex newIndex = this->index(d->root->childCount()-1,0,QModelIndex());
            emit layoutChanged();
            emit dataChanged(newIndex,newIndex);
        }

    }
}

/**
 * return a list of strings that represents the currently shown columns
 */
QStringList medDatabaseModel::columnNames() const
{
    if ( d->columnNames.isEmpty() )
    {
        QStringList ret;
#if QT_VERSION > 0x0406FF
        ret.reserve( d->DataCount );
#endif
        for (int i(0); i<d->DataCount; ++i)
            ret.append(QString());

        int i=0;
        ret[i++] = tr("Patient / Study / Serie");
        ret[i++] = tr("Date of birth");
        ret[i++] = tr("Gender");
        ret[i++] = tr("Slice Count");
        ret[i++] = tr("Age");
        ret[i++] = tr("Modality");
        ret[i++] = tr("Date acquired");
        ret[i++] = tr("Date imported");
        ret[i++] = tr("Referee");
        ret[i++] = tr("Performer");
        ret[i++] = tr("Institution");
        ret[i++] = tr("Report");
        d->columnNames = ret;
    }
    return d->columnNames;
}


QVariant medDatabaseModel::convertQStringToQVariant(QString keyName, QString value)
{
    const medMetaDataKeys::Key *key = medMetaDataKeys::Key::fromKeyName(keyName.toStdString().c_str());
    QVariant res;
    QDate date;

    switch(key->type())
    {
        case QVariant::Int:
            res = value.toInt();
            break;
        case QVariant::UInt:
            res = value.toUInt();
            break;
        case QVariant::Date:
            //TODO: ne marche pas pour tous les formats de date
            date = QDate::fromString(value);
            res = date;
            break;
        case QVariant::Char:
            if(value.isEmpty())
                res = QVariant(QChar());
            else
                res = value.at(0);
            break;
        default:
            res = value;
    }
    return res;
}

void medDatabaseModel::changePersistenIndexAndSubIndex(QModelIndex index)
{
    for(int i=0; i<columnCount(); i++)
    {
        QModelIndex tempIndex = index.sibling(index.row(),i);

        if(i==0)
        {
            int j=0;
            while(tempIndex.child(j,0).isValid())
            {
                changePersistenIndexAndSubIndex(tempIndex.child(j,0));
                j++;
            }
        }
        changePersistentIndex(tempIndex, QModelIndex());
    }
}

