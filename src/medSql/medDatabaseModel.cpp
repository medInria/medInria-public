/* medDatabaseModel.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct 14 22:57:50 2008 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Sep 29 13:53:12 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 430
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <QtCore>
#include <QtSql>

#include <dtkCore/dtkGlobal.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseItem.h>
#include <medSql/medDatabaseModel.h>

// /////////////////////////////////////////////////////////////////
// medDatabaseModelPrivate
// /////////////////////////////////////////////////////////////////

class medDatabaseModelPrivate
{
public:
    medDatabaseItem *item(const QModelIndex& index) const;

public:
    medDatabaseItem *root;
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
    QList<QVariant> data = QList<QVariant>() << "id" << "name";

    d->root = new medDatabaseItem("", data, data);

    populate(d->root);

    connect(medDatabaseController::instance(), SIGNAL(updated()), this, SLOT(clear()));
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
        return d->root->data(section);

    return QVariant();
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
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled;
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
 * \return true if the update is successful, flase otherwise.
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
        QVariant table     = item->table();
        QVariant attribute = item->attribute(index.column());
        QVariant value     = item->value(index.column());
        QVariant id        = item->value(0);

        QSqlQuery query(*(medDatabaseController::instance()->database()));
        query.prepare(QString("UPDATE %1 SET %2 = :value WHERE id = :id")
                    .arg(table.toString())
                    .arg(attribute.toString()));
        query.bindValue(":value", value);
        query.bindValue(":id", id);
        if(!query.exec())
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;
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
    return QAbstractItemModel::supportedDropActions() | Qt::MoveAction;
}

bool medDatabaseModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    qDebug() << __func__ << data->formats();

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
    
void medDatabaseModel::clear(void)
{
    removeRows(0, rowCount());

    reset();

    populate(d->root);
}

//! Model population.
/*! 
 * This method fills the model in with the data. The actual data is
 * contained within an medDatabaseItem and the later is accessed from
 * an index using the QModelIndex::internalPointer() method.
 * 
 * \param root The root item of the model.
 */

void medDatabaseModel::populate(medDatabaseItem *root)
{
    QSqlQuery ptQuery(*(medDatabaseController::instance()->database()));
    ptQuery.prepare("SELECT * FROM patient");
    if(!ptQuery.exec())
        qDebug() << DTK_COLOR_FG_RED << ptQuery.lastError() << DTK_NOCOLOR;

    while(ptQuery.next()) { // ---------------------------------------------------- Retrieving patients
        QVariant   ptId = ptQuery.value(0);
        QVariant ptName = ptQuery.value(1);

        medDatabaseItem *ptItem = new medDatabaseItem("patient",
                QList<QVariant>() << "id" << "name",
                QList<QVariant>() << ptId << ptName, root);

        QSqlQuery stQuery(*(medDatabaseController::instance()->database()));
        stQuery.prepare("SELECT * FROM study WHERE patient = :id");
        stQuery.bindValue(":id", ptId);
        if(!stQuery.exec())
            qDebug() << DTK_COLOR_FG_RED << stQuery.lastError() << DTK_NOCOLOR;

        while(stQuery.next()) { // ------------------------------------------------- Retrieving studies
            QVariant   stId = stQuery.value(0);
            QVariant stName = stQuery.value(2);
            
            medDatabaseItem *stItem = new medDatabaseItem("study",
                    QList<QVariant>() << "id" << "name",
                    QList<QVariant>() << stId << stName, ptItem);
            
            QSqlQuery seQuery(*(medDatabaseController::instance()->database()));
            seQuery.prepare("SELECT * FROM series WHERE study = :id");
            seQuery.bindValue(":id", stId);
            if(!seQuery.exec())
                qDebug() << DTK_COLOR_FG_RED << seQuery.lastError() << DTK_NOCOLOR;

            while(seQuery.next()) { // ---------------------------------------------- Retrieving series
                QVariant   seId = seQuery.value(0);
                QVariant seSize = seQuery.value(2);
                QVariant seName = seQuery.value(3);
                
                medDatabaseItem *seItem = new medDatabaseItem("series",
                        QList<QVariant>() << "id" << "name",
                        QList<QVariant>() << seId << seName, stItem);
                
                QSqlQuery imQuery(*(medDatabaseController::instance()->database()));
                imQuery.prepare("SELECT * FROM image WHERE series = :id");
                imQuery.bindValue(":id", seId);
                if(!imQuery.exec())
                    qDebug() << DTK_COLOR_FG_RED << imQuery.lastError() << DTK_NOCOLOR;
                
                while(imQuery.next()) { // ------------------------------------------ Retrieving images
                    QVariant   imId = imQuery.value(0);
                    QVariant imSize = imQuery.value(2);
                    QVariant imName = imQuery.value(3);
                    QVariant imThum = imQuery.value(4);

                    medDatabaseItem *imItem = new medDatabaseItem("image",
                            QList<QVariant>() << "id" << "name" << "thumbnail",
                            QList<QVariant>() << imId << imName << imThum, seItem);
                    
                    seItem->append(imItem);
                }            
                stItem->append(seItem);
            }
            ptItem->append(stItem);
        }
        root->append(ptItem);
    }
}
