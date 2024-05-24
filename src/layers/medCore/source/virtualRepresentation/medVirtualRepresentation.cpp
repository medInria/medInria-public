#include "medVirtualRepresentation.h"

#include <medDataImporter.h>
#include <medDataExporter.h>

#include <medDataHub.h>

#include <medStringParameter.h>
#include <medIntParameter.h>

#include <QMultiMap>
#include <QQueue>

#include <QDir>
#include <QDirIterator>

#define DATATYPE_ROLE  103
#define DATATYPE_ROLE_DATASET 0
#define DATATYPE_ROLE_FOLDER 1
#define DATATYPE_ROLE_BOTH  2

class medVirtualRepresentationPrivate
{
public:
    medVirtualRepresentationPrivate(medVirtualRepresentation *parent) : basePath("", parent), dayBeforeRemove("Expiration delay", parent) {};
    ~medVirtualRepresentationPrivate() = default;

    medStringParameter basePath;
    medIntParameter    dayBeforeRemove;

    QMultiMap<QString, QString>      uriToJsonPaths;
    QMap<QString, medAbstractData *> uriToAbstractData;
};



/**
 * @brief Constructs a new medVirtualRepresentation object.
 * @param parent The optional parent object.
 */
medVirtualRepresentation::medVirtualRepresentation(QObject *parent): QStandardItemModel(parent), d(new medVirtualRepresentationPrivate(this))
{    
    setColumnCount(1);
    setHorizontalHeaderLabels(QStringList("description"));
    d->basePath.setValue(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName() + "/virtualRepresentation");
    connect(&d->basePath, &medStringParameter::valueChanged, this, &medVirtualRepresentation::fetch);
    connect(this, &QStandardItemModel::itemChanged, this, &medVirtualRepresentation::renameByItem);
    fetch(d->basePath.value());
}

/**
  * @brief Destructs the medVirtualRepresentation object.
  */
medVirtualRepresentation::~medVirtualRepresentation()
{
    delete d;
}


// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
// //////////////  File system handling                                ///////
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////

/**
 * @brief Removes an item (in the model and the file system).
 * @param index The index of the item to remove.
 * @return True if the item was removed successfully, false otherwise.
 */
bool medVirtualRepresentation::remove(QModelIndex index)
{
    bool bRes = false;

    QString path = getPath(index);
    bRes = removeRow(index.row(), index.parent());
    if (bRes)
    {
        removeFSFile(path);
        removeFSDir(path);
    }

    return bRes;
}
/**
 * @brief Renames an item.
 * @param index The index in model of the item to rename.
 * @param newName The new name of the item.
 * @return True if the item was renamed successfully, false otherwise.
 */
bool medVirtualRepresentation::rename(QModelIndex index, QString newName)
{
    bool bRes = false;

    QDir dir(getPath(index.parent()));
    bRes = dir.rename(data(index, DATANAME_ROLE).toString(), newName);

    return bRes;
}

/**
 * @brief Creates a new item (directory type).
 * @param parent The parent index of the new item.
 * @param dirName The name of the new item.
 * @return True if the item was created successfully, false otherwise.
 */
bool medVirtualRepresentation::create(QModelIndex parent, QString dirName)
{
    bool bRes = false;

    QStandardItem * pParentItem = nullptr;
    if (parent.isValid())
    {
        pParentItem = itemFromIndex(parent);
    }
    else
    {
        pParentItem = invisibleRootItem();
    }
    auto ptrItem = new QStandardItem(dirName);
    ptrItem->setData(DATATYPE_ROLE_FOLDER, DATATYPE_ROLE);
    ptrItem->setData(dirName, DATANAME_ROLE);

    pParentItem->appendRow(ptrItem);
    bRes = createFSDirectory(getPath(parent), dirName);

    emit expandData(parent, true);

    if (dirName == "tmp")
    {
        emit editIndex(indexFromItem(ptrItem));
    }

    return bRes;
}


// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
// //////////////  Drag and drop section                               ///////
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
/**
 * @brief Handles dropped MIME data.
 *
 * This function is called when MIME data is dropped onto the model.
 * It determines whether the data can be dropped and, if so, performs the appropriate action (e.g., copying or moving the data).
 *
 * Qt's documentation says :
 * Handles the data supplied by a drag and drop operation that ended with the given action.
 * Returns true if the data and action were handled by the model; otherwise returns false.
 * The specified row, column and parent indicate the location of an item in the model where the operation ended. 
 * It is the responsibility of the model to complete the action at the correct location.
 * For instance, a drop action on an item in a QTreeView can result in new items either being inserted as children of the item specified by row, column, and parent, or as siblings of the item.
 * When row and column are -1 it means that the dropped data should be considered as dropped directly on parent. Usually this will mean appending the data as child items of parent.
 * If row and column are greater than or equal zero, it means that the drop occurred just before the specified row and column in the specified parent.
 * The mimeTypes() member is called to get the list of acceptable MIME types.
 * This default implementation assumes the default implementation of mimeTypes(), which returns a single default MIME type.
 * If you reimplement mimeTypes() in your custom model to return multiple MIME types, you must reimplement this function to make use of them.
 * 
 * @param data The MIME data that was dropped.
 * @param action The drop action that was requested (e.g., Qt::CopyAction, Qt::MoveAction).
 * @param row The row where the data was dropped.
 * @param column The column where the data was dropped.
 * @param parent The parent index of the item where the data was dropped.
 * @return True if the data was dropped successfully, false otherwise.
 *
 * @details Reimplements: QAbstractItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent).
 */
bool medVirtualRepresentation::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
    bool bRes = false;
    
    if (data->hasFormat("med/index2"))
    {
        auto indexList = medDataIndex::readMimeDataMulti(data);
        
        for (auto & index : indexList)
        {
            switch (action)
            {
                case Qt::CopyAction:
                case Qt::LinkAction:
                    addData(index, "", nullptr, parent); break;
                case Qt::MoveAction:
                    //TODO move
                default:
                    break;
            }
        }
    }
    else if (data->hasFormat("text/uri-list"))
    {
        for (const QUrl & url : data->urls())
        {
            addData(fileSysPathToIndex(url.toLocalFile()), "", nullptr, parent);
        }
    }

    return bRes;
}

/**
 * @brief Creates MIME data for the specified model indexes.
 *
 * This function is called when the user drags data from the model.
 * It creates a MIME data object that contains the data to be dragged.
 *
 * Qt's documentation says :
 * Returns an object that contains serialized items of data corresponding to the list of indexes specified.
 * The format used to describe the encoded data is obtained from the mimeTypes() function.
 * This default implementation uses the default MIME type returned by the default implementation of mimeTypes().
 * If you reimplement mimeTypes() in your custom model to return more MIME types, reimplement this function to make use of them.
 * If the list of indexes is empty, or there are no supported MIME types, nullptr is returned rather than a serialized empty list.
 * 
 * @param indexes The list of model indexes to create MIME data for.
 * @return A QMimeData object containing the data to be dragged, or nullptr if no data can be dragged.
 * 
 * @details Reimplements: QAbstractItemModel::mimeData(const QModelIndexList &indexes) const.
 */
QMimeData * medVirtualRepresentation::mimeData(const QModelIndexList & indexes) const
{
    QMimeData *mimeData = new QMimeData;
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QList<QUrl> urls;

    for (QModelIndex const &index : indexes)
    {
        if (index.isValid() && index.column() == 0)
        {
            QString origin = itemFromIndex(index)->data(DATAORIGIN_ROLE).toString();
            QString dataUri = itemFromIndex(index)->data(DATAURI_ROLE).toString();
            if (origin == DATAORIGIN_ROLE_SOURCE)
            {
                if (!encodedData.isEmpty())
                {
                    encodedData.append('\0');
                }
                encodedData.append(dataUri);
            }
            else if (origin == DATAORIGIN_ROLE_FILESYSTEM)
            {
                //urls.append(QUrl::fromLocalFile(dataUri));
                if (!encodedData.isEmpty())
                {
                    encodedData.append('\0');
                }
                encodedData.append(dataUri);
            }
            else if (origin == DATAORIGIN_ROLE_CACHE)
            {

            }
        }
    }
    mimeData->setUrls(urls);

    mimeData->setData("med/index2", encodedData);
    return mimeData;
}

/**
 * @brief Returns a list of MIME types that the model can provide for dragging.
 *
 * This function is used to determine what types of data the model can drag.
 * Here supported types are med/index2 and text/uri-list
 * 
 * Qt's documentation says :
 * Returns the list of allowed MIME types. By default, the built-in models and views use an internal MIME type: application/x-qabstractitemmodeldatalist.
 * When implementing drag and drop support in a custom model, if you will return data in formats other than the default internal MIME type, reimplement this function to return your list of MIME types.
 * If you reimplement this function in your custom model, you must also reimplement the member functions that call it: mimeData() and dropMimeData().
 * 
 * @return A list of MIME types that the model can provide for dragging.
 *
 * @details Reimplements: QAbstractItemModel::mimeTypes() const.
 */
QStringList medVirtualRepresentation::mimeTypes() const
{
    QStringList types;
    types << "med/index2" << "text/uri-list";
    return types;
}

/**
 * @brief Returns the drop actions that are supported by the model.
 *
 * This function determines what types of drop actions (e.g., copying, moving) are supported by the model.
 *
 * Qt's documentation says :
 * Returns the drop actions supported by this model.
 * The default implementation returns Qt::CopyAction. 
 * Reimplement this function if you wish to support additional actions. 
 * You must also reimplement the dropMimeData() function to handle the additional operations.
 * 
 * @return A set of Qt::DropAction flags indicating the supported drop actions.
 *
 * @details Reimplements: QAbstractItemModel::supportedDropActions() const.
 */
Qt::DropActions medVirtualRepresentation::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}


// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
// //////////////  Other functions section                             ///////
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////

/**
 * @brief Converts a medDataIndex to a QModelIndex.
 *
 * This function translates an index from the medInria data store to a corresponding index in the model.
 * This allows you to interact with data in the model using the medInria indexing scheme.
 *
 * @param index The medDataIndex to convert.
 * @return The corresponding QModelIndex in the model, or an invalid index if the conversion fails.
 */
QModelIndex medVirtualRepresentation::getModelIndex(medDataIndex index)
{
    QModelIndex indexRes;
    QString indexAsString = index.asString();
    if (d->uriToJsonPaths.contains(indexAsString))
    {
        QString jsonPath = d->uriToJsonPaths.value(indexAsString);
        indexRes = getIndex(jsonPath);
    }

    return indexRes;
}

/**
 * @brief Retrieves the list of medVirtualRepresentation's parameters .
 *
 * This function retrieves the list of all the medAbstractParameter objects of medVirtualRepresentation.
 * These parameters may be used to configure or control the behavior of medVirtualRepresentation.
 *
 * @return A list of medAbstractParameter objects.
 */
QList<medAbstractParameter*> medVirtualRepresentation::getParams()
{
    return {& d->basePath, & d->dayBeforeRemove};
}


// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
// //////////////  Public slots section                                ///////
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////

/**
 * @brief Adds generated data to the model.
 * @param data The data to add.
 * @param name The optional name of the data.
 */
void medVirtualRepresentation::addGeneratedData(medAbstractData * data, QString name)
{
    if (data)
    {
        //Est-ce qu'il existe deja ?
        QString dataName;

        if (!name.isEmpty())
        {
            dataName = name;
        }
        else if (!data->getExpectedName().isEmpty())
        {
            dataName = data->getExpectedName();
        }
        else
        {
            dataName = "tmp data";
        }

        QStandardItem *item = invisibleRootItem();

        QString tmpName = dataName;
        QString dataPath = getPath();
        int     indexPlace = 0;

        computeNameAndPlace(item, "", dataName, indexPlace, tmpName);


        QStandardItem *newItem = new QStandardItem(tmpName);
        newItem->setData(tmpName, DATANAME_ROLE);
        newItem->setData(DATAORIGIN_ROLE_CACHE, DATAORIGIN_ROLE);
        newItem->setData(tmpName, DATAURI_ROLE);
        newItem->setData(false, DATAGARBAGEABLE_ROLE);
        newItem->setData(DATATYPE_ROLE_DATASET, DATATYPE_ROLE);

        item->insertRow(indexPlace, newItem);

        auto index = newItem->index();
        QJsonObject jsonObj;
        if (writeJson(index, jsonObj))
        {
            writeFileEntry(jsonObj, dataPath, dataName);
        }

        QString uri = dataName;
        insertData(data, uri, dataPath, tmpName);
    }
}

/**
 * @brief Adds data.
 * @param medIndex The index of the data in medInria.
 * @param volumeName The optional volume name of the data.
 * @param data The optional data object to add.
 * @param parent The optional parent index of the new item.
 */
void medVirtualRepresentation::addData(medDataIndex medDataIndex, QString volumeName, medAbstractData * data, const QModelIndex & parent)
{
    QStandardItem *item = nullptr;

    if (parent.isValid() && parent.model() == this)
    {
        item = itemFromIndex(parent);
    }
    else
    {
        item = invisibleRootItem();
    }

    QString dataName;
    QString datOriginRole;
    if (medDataIndex.sourceId() == "fs")
    {
        datOriginRole = DATAORIGIN_ROLE_FILESYSTEM;
        if (!volumeName.isEmpty())
        {
            dataName = volumeName;
        }
        else
        {
            QStringList paths = indexToFileSysPath(medDataIndex.asString());
            if (paths.size() == 1)
            {
                dataName = QFileInfo(paths[0]).fileName();
            }
            else if (paths.size() > 1)
            {
                QStringList pathAsList = paths[0].split('/', QString::SkipEmptyParts);
                dataName = pathAsList[pathAsList.size() - 2]; //the name is the name of last folder
            }
        }

    }
    else
    {
        datOriginRole = DATAORIGIN_ROLE_SOURCE;
        dataName = medDataHub::instance()->getDataName(medDataIndex);
    }


    QString tmpName = dataName;
    QString dataPath = getPath(parent);
    int  indexPlace = 0;

    computeNameAndPlace(item, medDataIndex.asString(), dataName, indexPlace, tmpName);

    QString uri = medDataIndex.asString();
    if (!d->uriToJsonPaths.contains(uri))
    {

        QStandardItem *newItem = new QStandardItem(tmpName);
        newItem->setData(tmpName, DATANAME_ROLE);
        newItem->setData(datOriginRole, DATAORIGIN_ROLE);
        newItem->setData(medDataIndex.asString(), DATAURI_ROLE);
        newItem->setData(false, DATAGARBAGEABLE_ROLE);
        newItem->setData(DATATYPE_ROLE_DATASET, DATATYPE_ROLE);

        item->insertRow(indexPlace, newItem);


        auto modelIndex = newItem->index();
        QJsonObject jsonObj;
        if (writeJson(modelIndex, jsonObj))
        {
            writeFileEntry(jsonObj, dataPath, dataName);
        }

        insertData(data, uri, dataPath, tmpName);
    }
}

/**
 * @brief Pins the data represented by the given model index.
 *
 * This function marks the data at the specified model index as pinned. Pinned data is typically not automatically removed from the virtual representation,
 * even if it is old or there is a lot of data.
 *
 * @param modelIndex The index of the data to pin.
 */
void medVirtualRepresentation::pinData(QModelIndex modelIndex)
{
    auto *ptrItem = itemFromIndex(modelIndex);
    if (ptrItem)
    {
        ptrItem->setData(false, DATAGARBAGEABLE_ROLE);
        //TODO save new state to filesystem
    }
}

/**
 * @brief Unpins the data represented by the given model index.
 *
 * This function reverses the effect of `pinData()`. Unpinned data may be automatically removed from the virtual representation under certain conditions.
 *
 * @param modelIndex The index of the data to unpin.
 */
void medVirtualRepresentation::unPinData(QModelIndex modelIndex)
{
    auto *ptrItem = itemFromIndex(modelIndex);
    if (ptrItem)
    {
        ptrItem->setData(true, DATAGARBAGEABLE_ROLE);
        //TODO save new state to filesystem
    }
}

/**
 * @brief Removes the data represented by the given model index.
 *
 * This function removes the data and its associated item from the virtual representation.
 *
 * @param modelIndex The index of the data to remove.
 */
void medVirtualRepresentation::removeData(QModelIndex modelIndex)
{
    //TODO
}

/**
 * @brief Reacts to a data being saved in medInria.
 *
 * This function is called when data represented by a medDataIndex is saved in medInria.
 * It removes any cached data associated with the index and replaces it with a URI pointing to the data in the source location.
 *
 * @param index The medDataIndex of the saved data.
 */
void medVirtualRepresentation::dataSaved(medDataIndex index)
{
    //TODO OK
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
// //////////////  JSon handling section                               ///////
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////

/**
 * @brief Fetches data from a JSON file.
 *
 * This function loads Virtual Representation content from a JSON files located at the specified path.
 * The path can be an empty string, in which case the default behavior is likely to apply.
 *
 * @param path The path to the JSON files. (default = "")
 * @return True if the data was fetched successfully, false otherwise.
 */
bool medVirtualRepresentation::fetch(QString const & pi_path)
{
    bool bRes = true;


    QString basePath = d->basePath.value();

    QDirIterator it(pi_path, QStringList() << "*.json", QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString aPath = it.next();
        QString aRelativePath = aPath.right(aPath.length() - (basePath.length() + 1));
        auto aPathSplited = aRelativePath.split('/');

        if (!QDir(aPath).exists())
        {
            QString dataOrigine;
            QString dataURI;
            bool dataGarbageable = false;

            bool bValidEntryFile = readFileEntry(aPath, dataOrigine, dataURI, dataGarbageable);

            if (bValidEntryFile)
            {
                QFileInfo fileInfo(aPath);
                QDateTime fileCreationTime = fileInfo.birthTime();
                QDateTime currentDate = QDateTime::currentDateTime();
                if (currentDate > fileCreationTime.addDays(d->dayBeforeRemove.value()) && dataGarbageable)
                {
                    removeFSFile(aPath);
                }
                else
                {
                    QModelIndex lastFolderIndex;

                    for (int i = 0; i < aPathSplited.size() - 1; ++i)
                    {
                        lastFolderIndex = createFolderIndex(aPathSplited.mid(0, i + 1));
                    }

                    QStandardItem * parentItem = lastFolderIndex.isValid() ? itemFromIndex(lastFolderIndex) : invisibleRootItem();
                    QStandardItem * child = nullptr;

                    QString qualifiedDataName = aPathSplited.last();
                    QString dataName = qualifiedDataName.left((qualifiedDataName.length() - 5));


                    for (int i = 0; i < parentItem->rowCount() && child == nullptr; ++i)
                    {
                        QStandardItem * aChild = parentItem->child(i);
                        if (aChild->data(Qt::DisplayRole).toString() == dataName)
                        {
                            child = aChild;
                        }
                    }

                    if (child == nullptr)
                    {
                        QStandardItem * newItem = new QStandardItem();
                        newItem->setData(dataName, Qt::DisplayRole);
                        newItem->setData(dataName, DATANAME_ROLE);
                        newItem->setData(dataOrigine, DATAORIGIN_ROLE);
                        newItem->setData(dataURI, DATAURI_ROLE);
                        newItem->setData(dataGarbageable, DATAGARBAGEABLE_ROLE);
                        newItem->setData(DATATYPE_ROLE_DATASET, DATATYPE_ROLE);
                        parentItem->insertRow(parentItem->rowCount(), newItem);
                    }
                    else
                    {
                        child->setData(dataOrigine, DATAORIGIN_ROLE);
                        child->setData(dataURI, DATAURI_ROLE);
                        child->setData(dataGarbageable, DATAGARBAGEABLE_ROLE);
                        child->setData(DATATYPE_ROLE_BOTH, DATATYPE_ROLE);
                    }

                    d->uriToJsonPaths.insert(dataURI, aPath);

                }
            }
        }
        else
        {
            createFolderIndex(aPathSplited);
        }
    }

    return bRes;
}

/**
 * @brief Reads information from a JSON file entry.
 *
 * This function reads information about a data entry from a JSON file.
 * It extracts the data origin, data URI, and garbage collection flag from the entry.
 *
 * @param filePath The path to the JSON file.
 * @param dataOrigine [OUT] A string to store the data origin retrieved from the file.
 * @param dataURI [OUT] A string to store the data URI retrieved from the file.
 * @param dataGarbadgeable [OUT] A boolean flag indicating whether the data can be automatically removed (garbage collectable).
 * @return True if the information was read successfully, false otherwise.
 */
bool medVirtualRepresentation::readFileEntry(QString filePath, QString &dataOrigine, QString &dataURI, bool &dataGarbadgeable)
{
    bool bRes = false;

    if (!filePath.isEmpty())
    {
        QFile entryFile(filePath);
        if (entryFile.open(QFile::ReadOnly))
        {
            QString content = entryFile.readAll();
            entryFile.close();
            bRes = readJson(content.toUtf8(), dataOrigine, dataURI, dataGarbadgeable);
        }
    }

    return bRes;
}

/**
 * @brief Writes information to a JSON file entry.
 *
 * This function writes information about a data entry to a JSON file.
 * It stores the provided data origin, data URI, and garbage collection flag in the entry.
 *
 * @param entry The QJsonObject containing the data to write.
 * @param filePath The path to the JSON file.
 * @param fileName The name of the file to write to.
 * @return True if the information was written successfully, false otherwise.
 */
bool medVirtualRepresentation::writeFileEntry(QJsonObject const & entry, QString filePath, QString fileName)
{
    bool bRes = false;

    if (!filePath.isEmpty() && !fileName.isEmpty())
    {
        QFile entryFile(filePath + '/' + fileName + ".json");
        if (entryFile.open(QFile::WriteOnly | QFile::Truncate))
        {
            QJsonDocument jsonDoc;
            jsonDoc.setObject(entry);
            QByteArray data = jsonDoc.toJson();
            bRes = entryFile.write(data) == data.length();
        }
    }

    return bRes;
}

/**
 * @brief Writes data from a model index to a JSON object.
 *
 * This function extracts data from the model at the specified index and stores it in a JSON object.
 * The format of the data in the JSON object is likely to be specific to the application.
 * 
 * This function is used to prepare a JSON to call medVirtualRepresentation::writeFileEntry.
 * 
 * @param index The model index of the data to write.
 * @param json [OUT] A reference to the QJsonObject to store the data in.
 * @return True if the data was written to the JSON object successfully, false otherwise.
 */
bool medVirtualRepresentation::writeJson(const QModelIndex & index, QJsonObject &json)
{
    bool bRes = index.isValid();

    if (bRes)
    {
        auto dataOrigine = data(index, DATAORIGIN_ROLE).toString();
        auto dataURI = data(index, DATAURI_ROLE).toString();
        auto dataGarbadgeable = data(index, DATAGARBAGEABLE_ROLE).toBool();

        if (dataOrigine == DATAORIGIN_ROLE_FILESYSTEM)
        {
            json.insert("uriType", QJsonValue::fromVariant(DATAORIGIN_ROLE_FILESYSTEM));
            json.insert("uri", QJsonValue::fromVariant(dataURI));
            json.insert("garbageable", QJsonValue::fromVariant(dataGarbadgeable));
        }
        else if (dataOrigine == DATAORIGIN_ROLE_SOURCE)
        {
            if (!dataURI.isEmpty())
            {
                json.insert("uriType", QJsonValue::fromVariant(DATAORIGIN_ROLE_SOURCE));
                json.insert("uri", QJsonValue::fromVariant(dataURI));
                json.insert("garbageable", QJsonValue::fromVariant(dataGarbadgeable));
            }
        }
        else if (dataOrigine == DATAORIGIN_ROLE_CACHE)
        {
            json.insert("uriType", QJsonValue::fromVariant(DATAORIGIN_ROLE_CACHE));
            json.insert("uri", QJsonValue::fromVariant(""));
            json.insert("garbageable", QJsonValue::fromVariant(false));
        }
        else
        {
            bRes = false;
        }
    }

    return bRes;
}

/**
 * @brief Reads data from a JSON byte array.
 *
 * This function parses a JSON byte array and extracts information about a data entry.
 * It retrieves the data origin, data URI, and garbage collection flag from the JSON data.
 * 
 * This function is used by medVirtualRepresentation::readFileEntry.
 *
 * @param array The QByteArray containing the JSON data.
 * @param dataOrigine [OUT] A string to store the data origin retrieved from the JSON data.
 * @param dataURI [OUT] A string to store the data URI retrieved from the JSON data.
 * @param dataGarbadgeable [OUT] A boolean flag indicating whether the data can be automatically removed (garbage collectable).
 * @return True if the information was read successfully, false otherwise.
 */
bool medVirtualRepresentation::readJson(const QByteArray & array, QString &dataOrigine, QString &dataURI, bool &dataGarbadgeable)
{
    bool bRes = false;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(array);
    QJsonObject json;


    if (jsonDoc.isObject())
    {
        auto obj = jsonDoc.object();

        if (obj.contains("uriType") && obj["uriType"].isString() && !obj["uriType"].toString().isEmpty() &&
            obj.contains("uri") && obj["uri"].isString() &&
            obj.contains("garbageable") && obj["garbageable"].isBool())
        {
            dataOrigine = obj["uriType"].toString();
            dataURI = obj["uri"].toString();
            dataGarbadgeable = obj["garbageable"].toBool();
            bRes = true;

            if (dataOrigine == DATAORIGIN_ROLE_FILESYSTEM)
            {
                if (QFile::exists(dataURI))
                {
                    json.insert("uriType", QJsonValue::fromVariant(DATAORIGIN_ROLE_FILESYSTEM));
                    json.insert("uri", QJsonValue::fromVariant(dataURI));
                    json.insert("garbageable", QJsonValue::fromVariant(dataGarbadgeable));
                }
            }
            else if (dataOrigine == DATAORIGIN_ROLE_SOURCE)
            {
                if (!dataURI.isEmpty())
                {
                    json.insert("uriType", QJsonValue::fromVariant(DATAORIGIN_ROLE_SOURCE));
                    json.insert("uri", QJsonValue::fromVariant(dataURI));
                    json.insert("garbageable", QJsonValue::fromVariant(dataGarbadgeable));
                }
            }
            else if (dataOrigine == DATAORIGIN_ROLE_CACHE)
            {
                json.insert("uriType", QJsonValue::fromVariant(DATAORIGIN_ROLE_CACHE));
            }
            else
            {
                bRes = false;
            }
        }
    }

    return bRes;
}

// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
// //////////////  File system handling                                ///////
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////

/**
 * @brief Creates a directory in the file system.
 *
 * This function creates a new directory at the specified path.
 * The directory will be used to store JSON files associated with the model data.
 *
 * @param path The path to the parent directory where the new directory will be created.
 * @param dirName The name of the new directory to create.
 * @return True if the directory was created successfully, false otherwise.
 */
bool medVirtualRepresentation::createFSDirectory(QString path, QString dirName)
{
    bool bRes = false;

    QString dirPath = path + '/' + dirName;
    QDir dir(dirPath);
    if (!path.isEmpty() && !dirName.isEmpty() && !dir.exists())
    {
        bRes = dir.mkpath(dirPath);
    }

    return bRes;
}

/**
 * @brief Renames a file system entry (file or directory).
 *
 * This function renames a file or directory at the specified path.
 * It is likely used to rename JSON files associated with the model data.
 *
 * @param path The path to the file or directory to rename.
 * @return True if the entry was renamed successfully, false otherwise.
 */
bool medVirtualRepresentation::renameFSEntry(QString path)
{
    //TODO

    QFileInfo fi(path);
    if (fi.exists())
    {
        if (fi.isDir())
        {

        }
        else if (fi.isFile())
        {
            fi.baseName();
            fi.completeSuffix();
        }
    }
    return false;
}

/**
 * @brief Removes a directory from the file system.
 *
 * This function deletes a directory at the specified path.
 * The directory is likely used to store JSON files associated with the model data.
 *
 * @param path The path to the directory to remove.
 * @return True if the directory was removed successfully, false otherwise.
 */
bool medVirtualRepresentation::removeFSDir(QString path) //c:/toto/titi/tata/T13D.json -> c:/toto/titi/tata/T13D/
{
    return QDir(path).removeRecursively();
}

/**
 * @brief Removes a file from the file system.
 *
 * This function deletes a file at the specified path.
 * The file is likely a JSON file associated with the model data.
 *
 * @param path The path to the file to remove.
 * @return True if the file was removed successfully, false otherwise.
 */
bool medVirtualRepresentation::removeFSFile(QString path)
{
    bool bRes = true;

    auto pathAsLst = path.split('/');
    QString fileNameFilter = pathAsLst.last() + ".*";
    pathAsLst.pop_back();
    path = pathAsLst.join('/');
    QDir dir(path, fileNameFilter, QDir::Name | QDir::IgnoreCase, QDir::Files);
    for (const QString & filename : dir.entryList())
    {
        bRes &= dir.remove(filename);
    }

    return bRes;
}

/**
 * @brief Moves a file system entry (file or directory).
 *
 * This function moves a file or directory from one location to another.
 * It is likely used to move JSON files associated with the model data.
 *
 * @param oldPath The path to the source file or directory.
 * @param newPath The path to the destination location.
 * @return True if the entry was moved successfully, false otherwise.
 */
bool medVirtualRepresentation::moveFSEntry(QString oldPath, QString newPath)
{
    //TODO
    return false;
}


// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
// //////////////  Cache section                                       ///////
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////

/**
 * @brief Writes data to the cache.
 *
 * This function stores the provided medAbstractData object in the cache.
 * The data is likely written to a file at the specified path with the given name.
 *
 * @param data The medAbstractData object to store in the cache.
 * @param path The path to the cache directory where the data will be stored.
 * @param name The name of the file to use for storing the cached data.
 * @return True if the data was written to the cache successfully, false otherwise.
 */
bool medVirtualRepresentation::writeCacheData(medAbstractData * data, QString path, QString name)
{
    //TODO
    return false;
}

/**
 * @brief Reads data from the cache.
 *
 * This function attempts to read cached data from a file at the specified path with the given name.
 * If successful, it creates a new medAbstractData object and populates it with the cached data.
 *
 * @param data [OUT] A reference to a medAbstractData pointer that will be set to the newly created object containing the cached data, or nullptr if the data could not be read.
 * @param path The path to the cache directory where the data is expected to be stored.
 * @param name The name of the file containing the cached data.
 * @return True if the data was read from the cache successfully, false otherwise.
 */
bool medVirtualRepresentation::readCacheData(medAbstractData *& data, QString path, QString name)
{
    //TODO
    return false;
}

// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
// //////////////  Internal routines section                           ///////
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////

/**
 * @brief Internal function for computing name and placement.
 *
 * This function is likely used internally by the class and is not intended to be called directly from user code.
 * It computes the name and placement of an item based on the provided information.
 *
 * @param item The QStandardItem object representing the item.
 * @param indexAsString A string representation of the medDataIndex.
 * @param dataName The name of the data associated with the item.
 * @param indexPlace [OUT] An integer reference to store the computed placement of the item.
 * @param tmpName [OUT] A string reference to store a temporary name.
 */
void medVirtualRepresentation::computeNameAndPlace(QStandardItem * item, const QString indexAsString, const QString &dataName, int &indexPlace, QString &tmpName)
{
    int  childCount = item->rowCount();
    int  iOrder = 0;
    bool bAlreadyExist = false;

    indexPlace = 0;

    do
    {
        bAlreadyExist = false;

        for (int i = indexPlace; i < childCount && !bAlreadyExist; ++i)
        {
            QStandardItem * oneChild = item->child(i);
            if (oneChild != nullptr)
            {
                QString childName = oneChild->data(Qt::DisplayRole).toString();
                QString childUri = oneChild->data(DATAURI_ROLE).toString();
                bAlreadyExist = indexAsString == childUri;

                if (iOrder)
                {
                    tmpName = dataName + "_" + QString::number(iOrder);
                }
                else
                {
                    tmpName = dataName;
                }

                if (childName < tmpName)
                {
                    indexPlace++;
                }
                if (tmpName == childName)
                {
                    iOrder++;
                    bAlreadyExist = true;
                }
            }
        }
    } while (bAlreadyExist);
}

/**
 * @brief Internal function for inserting data.
 *
 * This function is likely used internally by the class and is not intended to be called directly from user code.
 * It inserts data into the model, handling aspects like URI, data path, and temporary name.
 *
 * @param data The medAbstractData object representing the data to insert.
 * @param uri [OUT] A string reference to store the URI of the data.
 * @param dataPath [OUT] A string reference to store the data path.
 * @param tmpName [OUT] A string reference to store a temporary name.
 */
void medVirtualRepresentation::insertData(medAbstractData * data, QString &uri, QString &dataPath, QString &tmpName)
{
    if (data)
    {
        if (!d->uriToAbstractData.contains(uri))
        {
            d->uriToAbstractData[uri] = data;
        }
    }
    d->uriToJsonPaths.insert(uri, dataPath + '/' + tmpName);
}


/**
 * @brief Internal function for creating a folder index.
 *
 * This function is likely used internally by the class and is not intended to be called directly from user code.
 * It creates a model index for a folder based on a list of strings representing the folder hierarchy.
 *
 * @param tree A QStringList containing the path components of the folder.
 * @return A QModelIndex representing the created folder, or an invalid index if the folder could not be created.
 */
QModelIndex medVirtualRepresentation::createFolderIndex(QStringList tree)
{
    QModelIndex indexRes;

    QStandardItem *item = invisibleRootItem();
    for (int i = 0; i < tree.size(); ++i)
    {
        auto indexLst = match(index(0, 0, indexRes), Qt::DisplayRole, tree[i], 1, Qt::MatchExactly);
        if (!indexLst.isEmpty())
        {
            indexRes = indexLst[0];
            item = itemFromIndex(indexRes);
            if (item->data(DATATYPE_ROLE).toInt() == DATATYPE_ROLE_DATASET)
            {
                item->setData(DATATYPE_ROLE_BOTH, DATATYPE_ROLE);
            }
        }
        else
        {
            QStandardItem * newItem = new QStandardItem();
            newItem->setData(tree[i], Qt::DisplayRole);
            newItem->setData(tree[i], DATANAME_ROLE);
            newItem->setData(DATATYPE_ROLE_FOLDER, DATATYPE_ROLE);
            item->insertRow(item->rowCount(), newItem);
            indexRes = indexFromItem(newItem);
        }
    }

    return indexRes;
}

/**
 * @brief Gets the path associated with a model index.
 *
 * This function retrieves the path associated with a given model index.
 * It likely traverses the model hierarchy to construct the path.
 *
 * @param index The model index (default = current index). (default = QModelIndex())
 * @return A string representing the path associated with the model index, or an empty string if the path cannot be determined.
 */
QString medVirtualRepresentation::getPath(QModelIndex index)
{
    QString resPath = d->basePath.value().replace('\\', '/');

    QStringList path;
    while (index.isValid())
    {
        path.push_front(data(index, Qt::DisplayRole).toString());
        index = index.parent();
    }

    if (!path.empty())
    {
        resPath = resPath + '/' + path.join('/');
    }

    return resPath;
}

/**
 * @brief Gets the model index for a JSON path.
 *
 * This function is likely used internally by the class and is not intended to be called directly from user code.
 * It retrieves a model index based on a path specified in JSON format.
 *
 * @param jsonPath A string representing the JSON path.
 * @return A QModelIndex representing the item at the specified JSON path, or an invalid index if the path is invalid.
 */
QModelIndex medVirtualRepresentation::getIndex(QString jsonPath)
{
    QModelIndex indexRes;

    QString basePath = d->basePath.value();
    if (jsonPath.startsWith(basePath))
    {
        jsonPath.remove(0, basePath.length());
        QStringList pathSplited = jsonPath.split('/');
        
        QModelIndex indexTmp;
        for (int i = 0; i < pathSplited.size(); ++i)
        {
            auto indexLst = match(index(0, 0, indexTmp), Qt::DisplayRole, pathSplited[i], 1, Qt::MatchExactly);
            if (!indexLst.isEmpty())
            {
                indexTmp = indexLst[0];
            }
        }
        indexRes = indexTmp;
    }

    return indexRes;
}

/**
 * @brief Removes data entries that are considered too old.
 *
 * This function likely iterates through cached, stored data entries, JSON files and removes those that are considered outdated based on some criteria.
 * This helps with managing storage space and keeping the data fresh.
 */
void medVirtualRepresentation::removeTooOldEntry()
{
    //TODO
}

/**
 * @brief Private slot for renaming an item based on the item object.
 *
 * This function is a private slot, meaning it can only be invoked from signals emitted within the class.
 * The connect is into the constructor.
 * It renames an item in the model based on information retrieved from the QStandardItem object itself.
 *
 * @param item The QStandardItem object representing the item to rename.
 */
void medVirtualRepresentation::renameByItem(QStandardItem * item)
{
    QModelIndex index = indexFromItem(item);
    QString name = item->data(Qt::DisplayRole).toString();

    rename(index, name);
}
