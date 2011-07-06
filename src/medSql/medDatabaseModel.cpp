/* medDatabaseModel.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct 14 22:57:50 2008 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Mar 19 20:43:04 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 447
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */
#include <QtGui>
#include <QtCore>
#include <QtSql>

#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseItem.h>
#include <medSql/medDatabaseModel.h>
#include <medSql/medDatabaseNonPersistentController.h>

#include <medCore/medAbstractDbController.h>
#include <medCore/medDbControllerFactory.h>
#include <medCore/medDataManager.h>
#include <medCore/medMetaDataHelper.h>

// /////////////////////////////////////////////////////////////////
// medDatabaseModelPrivate
// /////////////////////////////////////////////////////////////////

class medDatabaseModelPrivate
{
public:
    medDatabaseItem *item(const QModelIndex& index) const;

public:
    medDatabaseItem *root;

    QList<QVariant> ptAttributes;  // Attributes displayed on Patient rows
    QList<QVariant> seAttributes;  // Attributes displayed on series rows.
    QList<QVariant> ptDefaultData;
    QList<QVariant> seDefaultData;

    QList<QVariant> data;
    QList<QString> columnNames;

    enum { DataCount = 19 };
};

medDatabaseItem *medDatabaseModelPrivate::item(const QModelIndex& index) const
{
    if (index.isValid()) {
        medDatabaseItem *item = static_cast<medDatabaseItem *>(index.internalPointer());
        if (item) 
        return item;
    }

    return root;
}

// /////////////////////////////////////////////////////////////////
// medDatabaseModel
// /////////////////////////////////////////////////////////////////

medDatabaseModel::medDatabaseModel(QObject *parent) : QAbstractItemModel(parent), d(new medDatabaseModelPrivate)
{
    QString NulString;
    const int dataCount = d->DataCount;

    d->ptAttributes = QList<QVariant>();
    d->ptAttributes.reserve(dataCount);
    for (int i(0); i<dataCount; ++i)
        d->ptAttributes.append(NulString);

    d->ptAttributes[0] = medMetaDataHelper::KEY_PatientName();
    d->ptAttributes[5] = medMetaDataHelper::KEY_BirthDate();
    d->ptAttributes[6] = medMetaDataHelper::KEY_Gender();

    d->seAttributes = QList<QVariant>();
    d->seAttributes.reserve(dataCount);
    for (int i(0); i<dataCount; ++i)
        d->seAttributes.append(NulString);
    d->seAttributes[1] = medMetaDataHelper::KEY_StudyDescription();
    d->seAttributes[2] = medMetaDataHelper::KEY_SeriesDescription();
    d->seAttributes[3] = medMetaDataHelper::KEY_Size();
    d->seAttributes[4] = medMetaDataHelper::KEY_Age();
    d->seAttributes[7] = medMetaDataHelper::KEY_SeriesDescription();
    d->seAttributes[8] = medMetaDataHelper::KEY_Modality();
    d->seAttributes[12] = medMetaDataHelper::KEY_AcquisitionDate();
    d->seAttributes[13] = medMetaDataHelper::KEY_ImportationDate();
    d->seAttributes[15] = medMetaDataHelper::KEY_Referee();
    d->seAttributes[16] = medMetaDataHelper::KEY_Performer();
    d->seAttributes[17] = medMetaDataHelper::KEY_Institution();
    d->seAttributes[18] = medMetaDataHelper::KEY_Report();

    d->data = QList<QVariant>();
    d->data.reserve(dataCount);
    for (int i(0); i<dataCount; ++i)
        d->data.append(NulString);

    d->ptDefaultData =  d->data;
    d->ptDefaultData[0] = tr("[No Patient Name]");

    d->seDefaultData =  d->data;
    d->seDefaultData[1] = tr("[No Study Name]");
    d->seDefaultData[2] = tr("[No Series Name]");

    d->root = new medDatabaseItem(medDataIndex(), d->data, d->data);

    populate(d->root);

    connect(medDatabaseController::instance(), SIGNAL(updated(medDataIndex)), this, SLOT(repopulate()));
    connect(medDatabaseNonPersistentController::instance(), SIGNAL(updated(medDataIndex)), this, SLOT(repopulate()));
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
    medDatabaseItem *parentItem;

    if (parent.column() > 0)
        return 0;
    
    if (!parent.isValid())
        parentItem = d->root;
    else
        parentItem = static_cast<medDatabaseItem *>(parent.internalPointer());
    
    return parentItem->childCount();
}

int medDatabaseModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<medDatabaseItem *>(parent.internalPointer())->columnCount();
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
    
    medDatabaseItem *item = static_cast<medDatabaseItem *>(index.internalPointer());

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
        if(medDatabaseItem *item = static_cast<medDatabaseItem *>(index.internalPointer()))
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
        if(medDatabaseItem *patientItem = static_cast<medDatabaseItem *>(patientIndex.internalPointer()))
            for(int j = 0; j < patientItem->childCount(); j++) {
                QModelIndex index = this->index(j, 0, patientIndex);
                if(medDatabaseItem *item = static_cast<medDatabaseItem *>(index.internalPointer()))
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
        if(medDatabaseItem *patientItem = static_cast<medDatabaseItem *>(patientIndex.internalPointer()))
            for(int j = 0; j < patientItem->childCount(); j++) {
                QModelIndex studyIndex = this->index(j, 0, patientIndex);
                if(medDatabaseItem *studyItem = static_cast<medDatabaseItem *>(studyIndex.internalPointer()))
                    for(int k = 0; k < studyItem->childCount(); k++) {
                        QModelIndex index = this->index(k, 0, studyIndex);
                        if(medDatabaseItem *item = static_cast<medDatabaseItem *>(index.internalPointer()))
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
        if(medDatabaseItem *patientItem = static_cast<medDatabaseItem *>(patientIndex.internalPointer()))
            for(int j = 0; j < patientItem->childCount(); j++) {
                QModelIndex studyIndex = this->index(j, 0, patientIndex);
                if(medDatabaseItem *studyItem = static_cast<medDatabaseItem *>(studyIndex.internalPointer()))
                    for(int k = 0; k < studyItem->childCount(); k++) {
                        QModelIndex seriesIndex = this->index(k, 0, studyIndex);
                        if(medDatabaseItem *seriesItem = static_cast<medDatabaseItem *>(seriesIndex.internalPointer()))
                            for(int l = 0; l < seriesItem->childCount(); l++) {
                                QModelIndex index = this->index(l, 0, seriesIndex);
                                if(medDatabaseItem *item = static_cast<medDatabaseItem *>(index.internalPointer()))
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

    medDatabaseItem *parentItem;
    if (!parent.isValid())
        parentItem = d->root;
    else
        parentItem = static_cast<medDatabaseItem *>(parent.internalPointer());

    medDatabaseItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex medDatabaseModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    medDatabaseItem *child = static_cast<medDatabaseItem *>(index.internalPointer());
    medDatabaseItem *parent = child->parent();

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
    
    medDatabaseItem *item = d->item(index);

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
    medDatabaseItem *parentItem = d->item(parent);

    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, d->root->columnCount());
    endInsertRows();

    return success;
}

bool medDatabaseModel::removeRows(int position, int rows, const QModelIndex& parent)
{
    medDatabaseItem *parentItem = d->item(parent);

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

    int beginRow;

    if (row != -1)
        beginRow = row;
    else if (parent.isValid())
        beginRow = parent.row();
    else
        beginRow = rowCount(QModelIndex());

    int rows = 0;

    for (int i = 0; i < data->urls().size(); ++i, ++rows)
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

void medDatabaseModel::populate(medDatabaseItem *root)
{
    typedef QList<int> IntList;
    medDataManager * dataManager = medDataManager::instance();
    IntList dataSources = dataManager->dataSourceIds();
    {
        // Initial Rearrangement, so that non-Persistent data sources are at the end.
        IntList npDataSources;
        for (IntList::iterator dataSourceIt(dataSources.begin()); dataSourceIt!= dataSources.end(); ) {
            medAbstractDbController * dbc = dataManager->controllerForDataSource(*dataSourceIt);
            if ( !dbc ) {
                dataSourceIt = dataSources.erase(dataSourceIt);
            } else if ( dbc->isPersistent(medDataIndex(*dataSourceIt)) ) {
                ++dataSourceIt;
            } else {
                npDataSources.push_back(*dataSourceIt);
                dataSourceIt = dataSources.erase(dataSourceIt);
            }
        }
        dataSources << npDataSources;
    }

    medDataIndex index;
    foreach( const int dataSourceId, dataSources ) {

        medAbstractDbController * dbc = dataManager->controllerForDataSource(dataSourceId);
        IntList patients = dbc->patients();

        // Iterate over patientIds for this data source
        foreach( const int ptId, patients ) {

            index = medDataIndex( dataSourceId, ptId );
            QList<QVariant> ptData = d->ptDefaultData;
            for (int i(0); i<d->DataCount; ++i) {
                QVariant attribute = d->ptAttributes[i];
                if ( !attribute.isNull() ) {
                    QString value =  dbc->metaData(index, attribute.toString() );
                    if ( !value.isEmpty() ) 
                        ptData[i] = value;
                }
            }
            medDatabaseItem *ptItem = new medDatabaseItem(index, d->ptAttributes, ptData, root);

            IntList studies = dbc->studies(ptId);

            // Iterate over studyIds for this patient
            foreach( const int stId, studies ) {

                IntList series = dbc->series(ptId, stId);

                // Iterate over series for this study
                foreach( const int seId, series ) {

                    index = medDataIndex( dataSourceId, ptId, stId, seId );
                    QList<QVariant> seData = d->seDefaultData;
                    for (int i(0); i<d->DataCount; ++i) {
                        QVariant attribute = d->seAttributes[i];
                        if ( !attribute.isNull() ) {
                            QString value =  dbc->metaData(index, attribute.toString() );
                            if ( !value.isEmpty() ) 
                                seData[i] = value;
                        }
                    }
                    medDatabaseItem *seItem = new medDatabaseItem(index, d->seAttributes, seData, ptItem);

                    ptItem->append(seItem);
                } // foreach series 
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
        ret.reserve( d->DataCount );
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



