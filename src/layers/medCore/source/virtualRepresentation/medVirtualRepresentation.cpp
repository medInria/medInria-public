#include "medVirtualRepresentation.h"

#include <medDataImporter.h>
#include <medDataExporter.h>

#include <medDataHub.h>

#include <medStringParameter.h>
#include <medIntParameter.h>

#include <QMultiMap>
#include <QQueue>

#include <QThread>
#include <QThreadPool>
#include <QRunnable>

#include <QDir>
#include <QDirIterator>

#define MEDDATA_ROLE   102  //Boolean indicate if a medAbstractData is attached directly or indirectly to an item
#define DATASTATE_ROLE 100  //String defined as below
#define DATASTATE_ROLE_DATANOTLOADED  "DataNotLoaded"
#define DATASTATE_ROLE_DATALOADING    "DataLoading"
#define DATASTATE_ROLE_DATALOADED     "DataLoaded"
#define DATASTATE_ROLE_DATANOTSAVED   "DataNotSaved"
#define DATASTATE_ROLE_DATACOMMITTED  "DataCommited"
#define DATASTATE_ROLE_DATAPUSHING    "DataPushing"
#define DATASTATE_ROLE_DATASAVED      "DataSaved"


#define DATAORIGIN_ROLE 200  //String defined as below
#define DATAORIGIN_ROLE_FILESYSTEM "FS"
#define DATAORIGIN_ROLE_SOURCE     "source"
#define DATAORIGIN_ROLE_CACHE      "cache"

#define DATAURI_ROLE    201  //String URI of the data
#define DATAGARBAGEABLE_ROLE 202  //Boolean indicates if the data can be automatically removed (if longtime or too much data)
#define DATAISDIRECTORY_ROLE 203  //Boolean indicates if an item is a pure directory

#define DATANAME_ROLE 300

#define INVALID_ENTRY "invalidEntry.json"

struct conversionTask
{
    bool readWrite;
    medAbstractData * data;
    QString path;
    QString name;
};


class conversionWorker : public QRunnable
{
public:
    void run()
    {
        while (!conversionQueue->empty())
        {
            conversionTask conv = conversionQueue->dequeue();
            if (conv.readWrite)
            {
                medDataImporter::convertSingleDataOnfly(conv.path);
                //TODO
            }
            else
            {
                QString filePath = conv.path + QDir::separator() + conv.name;
                if (medDataExporter::convertSingleDataOnfly(conv.data, filePath))
                {
                    //TODO
                }
            }
        }
        QThread::currentThread()->wait(1000);
    }

    QQueue<conversionTask> *conversionQueue;
};

class medVirtualRepresentationPrivate
{
public:
    medVirtualRepresentationPrivate(medVirtualRepresentation *parent) : basePath("", parent), dayBeforeRemove("Expiration delay", parent) {};
    ~medVirtualRepresentationPrivate() = default;

    medStringParameter basePath;
    medIntParameter    dayBeforeRemove;
    QMap<QString, QPair<QStringList, QStringList> > flatTree; //Path, Dirs list, files list

    QMap<QString, medAbstractData *> jsonPathsToAbstractData;
    QMultiMap<QString, QString>      uriToJsonPaths;
    QMap<QString, medAbstractData *> uriToAbstractData;

    QQueue<conversionTask> conversionQueue;
};





medVirtualRepresentation::medVirtualRepresentation(QObject *parent): QStandardItemModel(parent), d(new medVirtualRepresentationPrivate(this))
{    
    setColumnCount(1);
    d->basePath.setValue(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName() + "/virtualRepresentation");
    connect(&d->basePath, &medStringParameter::valueChanged, this, &medVirtualRepresentation::fetch);
    connect(this, &QStandardItemModel::itemChanged, this, &medVirtualRepresentation::renameByItem);
    fetch(d->basePath.value());
}                       

medVirtualRepresentation::~medVirtualRepresentation()
{
    delete d;
}



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

bool medVirtualRepresentation::rename(QModelIndex index, QString newName)
{
    bool bRes = false;

    QDir dir(getPath(index.parent()));    
    bRes = dir.rename(data(index, DATANAME_ROLE).toString(), newName);

    return bRes;
}

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
    ptrItem->setData(true, DATAISDIRECTORY_ROLE);
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

bool medVirtualRepresentation::renameFSEntry(QString path)
{
    return false;
}

bool medVirtualRepresentation::removeFSDir(QString path) //c:/toto/titi/tata/T13D.json -> c:/toto/titi/tata/T13D/
{
    return QDir(path).removeRecursively();
}

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

bool medVirtualRepresentation::moveFSEntry(QString oldPath, QString newPath)
{
    return false;
}




//TODO regarder pour les 4 méthodes suivantes ce qui a été fait pour les medSourceModel
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
                    addDataFromSource(index, nullptr, parent); break;
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
            addDataFromFile(url.toLocalFile(), nullptr, parent);
        }
    }

    return bRes;
}

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
                urls.append(QUrl::fromLocalFile(dataUri));
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

QStringList medVirtualRepresentation::mimeTypes() const
{
    QStringList types;
    types << "med/index2" << "text/uri-list";
    return types;
}

Qt::DropActions medVirtualRepresentation::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

QList<medAbstractParameter*> medVirtualRepresentation::getParams()
{
    return {& d->basePath, & d->dayBeforeRemove};
}





bool medVirtualRepresentation::fetch(QString const & pi_path)
{
    bool bRes = true;


    QString basePath = d->basePath.value();

    QDirIterator it(pi_path, QStringList() << "*.json", QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString aPath = it.next(); 
        QString aRelativePath = aPath.right(aPath.length()-(basePath.length()+1));
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
                        parentItem->insertRow(parentItem->rowCount(), newItem);
                    }
                    else
                    {

                        child->setData(dataOrigine, DATAORIGIN_ROLE);
                        child->setData(dataURI, DATAURI_ROLE);
                        child->setData(dataGarbageable, DATAGARBAGEABLE_ROLE);
                    }

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


void medVirtualRepresentation::dataSaved(medDataIndex index)
{
    //TODO OK
}

void medVirtualRepresentation::addGeneratedData(medAbstractData * data, QString name)
{
    if (data)
    {
        //Est-ce qu'il exist déjà ?
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

        item->insertRow(indexPlace, newItem);

        auto index = newItem->index();
        QJsonObject jsonObj;
        if (writeJson(index, jsonObj))
        {
            writeFileEntry(jsonObj, dataPath, dataName);
        }

        QString uri = dataName;
        if (data)
        {
            if (!d->uriToAbstractData.contains(uri))
            {
                d->uriToAbstractData[uri] = data;
            }
        }
        d->uriToJsonPaths.insert(uri, dataPath + '/' + tmpName);
    }
}

void medVirtualRepresentation::addDataFromSource(medDataIndex dataIndex, medAbstractData * data, const QModelIndex & parent)
{

    //Est-ce qu'il exist déjà ?

    QStandardItem *item = nullptr;

    if (parent.isValid() && parent.model() == this)
    {
        item = itemFromIndex(parent);
    }
    else
    {
        item = invisibleRootItem();
    }

    QString dataName = medDataHub::instance()->getDataName(dataIndex);
    QString tmpName = dataName;
    QString dataPath = getPath(parent);
    int  indexPlace = 0;

    computeNameAndPlace(item, dataIndex.asString(), dataName, indexPlace, tmpName);


    QStandardItem *newItem = new QStandardItem(tmpName);
    newItem->setData(tmpName, DATANAME_ROLE);
    newItem->setData(DATAORIGIN_ROLE_SOURCE, DATAORIGIN_ROLE);
    newItem->setData(dataIndex.asString(), DATAURI_ROLE);
    newItem->setData(false, DATAGARBAGEABLE_ROLE);

    item->insertRow(indexPlace, newItem);
    
    
    auto modelIndex = newItem->index();
    QJsonObject jsonObj;
    if (writeJson(modelIndex, jsonObj))
    {
        writeFileEntry(jsonObj, dataPath, dataName);
    }


    QString uri = dataIndex.asString();

    d->uriToJsonPaths.insert(uri, dataPath + '/' + tmpName);
}

void medVirtualRepresentation::addDataFromFile(QString path, medAbstractData * data, const QModelIndex & parent)
{

    //Est-ce qu'il exist déjà ?

    QStandardItem *item = nullptr;

    if (parent.isValid() && parent.model() == this)
    {
        item = itemFromIndex(parent);
    }
    else
    {
        item = invisibleRootItem();
    }

    QString dataName = QFileInfo(path).fileName();
    QString tmpName = dataName;
    QString dataPath = getPath(parent);
    int     indexPlace = 0;

    computeNameAndPlace(item, path, dataName, indexPlace, tmpName);


    QStandardItem *newItem = new QStandardItem(tmpName);
    newItem->setData(tmpName, DATANAME_ROLE);
    newItem->setData(DATAORIGIN_ROLE_FILESYSTEM, DATAORIGIN_ROLE);
    newItem->setData(path, DATAURI_ROLE);
    newItem->setData(false, DATAGARBAGEABLE_ROLE);

    item->insertRow(indexPlace, newItem);

    auto index = newItem->index();
    QJsonObject jsonObj;
    if (writeJson(index, jsonObj))
    {
        writeFileEntry(jsonObj, dataPath, dataName);
    }

    QString uri = path;
    if (data)
    {
        if (!d->uriToAbstractData.contains(uri))
        {
            d->uriToAbstractData[uri] = data;
        }
    }
    d->uriToJsonPaths.insert(uri, dataPath + '/' + tmpName);
}

void medVirtualRepresentation::pinData(QModelIndex modelIndex)
{
    auto *ptrItem = itemFromIndex(modelIndex);
    if (ptrItem)
    {
        ptrItem->setData(false, DATAGARBAGEABLE_ROLE);
        //TODO save new state to filesystem
    }
}

void medVirtualRepresentation::unPinData(QModelIndex modelIndex)
{
    auto *ptrItem = itemFromIndex(modelIndex);
    if (ptrItem)
    {
        ptrItem->setData(true, DATAGARBAGEABLE_ROLE);
        //TODO save new state to filesystem
    }
}

void medVirtualRepresentation::removeData(QModelIndex modelIndex)
{
    //TODO
}


void medVirtualRepresentation::renameByItem(QStandardItem * item)
{
    QModelIndex index = indexFromItem(item);
    QString name = item->data(Qt::DisplayRole).toString();

    rename(index, name);
}
bool medVirtualRepresentation::writeCacheData(medAbstractData * data, QString path, QString name)
{
    return false;
}

bool medVirtualRepresentation::readCacheData(medAbstractData *& data, QString path, QString name)
{
    return false;
}

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
            item->setData(true, DATAISDIRECTORY_ROLE);
        }
        else
        {
            QStandardItem * newItem = new QStandardItem();
            newItem->setData(tree[i], Qt::DisplayRole);
            newItem->setData(tree[i], DATANAME_ROLE);
            newItem->setData(true, DATAISDIRECTORY_ROLE);
            item->insertRow(item->rowCount(), newItem);
            indexRes = indexFromItem(newItem);
        }
    }

    return indexRes;
}

void medVirtualRepresentation::removeTooOldEntry()
{
    //TODO
}


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

QModelIndex medVirtualRepresentation::getIndex(QString path)
{
    QModelIndex indexRes;

    QString basePath = d->basePath.value();
    if (path.startsWith(basePath))
    {
        path.remove(0, basePath.length());
        QStringList pathSplited = path.split('/');
        
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
