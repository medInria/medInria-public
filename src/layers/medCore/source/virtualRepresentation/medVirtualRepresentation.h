#pragma once
#include <QJsonObject>
#include <QStandardItemModel>

#include <medAbstractParameter.h>
#include <medAbstractData.h>
#include <medDataIndex.h>

#define DATAORIGIN_ROLE 200  //String defined as below
#define DATAORIGIN_ROLE_FILESYSTEM "FS"
#define DATAORIGIN_ROLE_SOURCE     "source"
#define DATAORIGIN_ROLE_CACHE      "cache"

#define DATAURI_ROLE    201  //String URI of the data
#define DATAGARBAGEABLE_ROLE 202  //Boolean indicates if the data can be automatically removed (if longtime or too much data)
//#define DATAISDIRECTORY_ROLE 203  //Boolean indicates if an item is a pure directory

#define DATANAME_ROLE 300

#define INVALID_ENTRY "invalidEntry.json"

class medVirtualRepresentationPrivate;
/**
 * @brief medVirtualRepresentation centralise toutes les données ouvertes dans medInria et elle assure aussi l'organization des données pour l'utilisateur.
 */
class MEDCORE_EXPORT medVirtualRepresentation : public QStandardItemModel
{
    Q_OBJECT
public:
    medVirtualRepresentation(QObject *parent = nullptr);
    ~medVirtualRepresentation();

    bool readFileEntry(QString filePath, QString &dataOrigine, QString &dataURI, bool &dataGarbadgeable);
    bool writeFileEntry(QJsonObject const & entry, QString filePath, QString fileName);


    bool remove(QModelIndex index);
    bool rename(QModelIndex index, QString newName);
    bool create(QModelIndex parent, QString dirName);


    
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    QMimeData *	mimeData(const QModelIndexList &indexes) const override;
    QStringList	mimeTypes() const override;
    Qt::DropActions	supportedDropActions() const override;
    
    QList<medAbstractParameter*> getParams();

    QModelIndex getModelIndex(medDataIndex index);

private:
    bool fetch(QString const &path = QString());

    bool writeJson(const QModelIndex  & index, QJsonObject &json);
    bool readJson (const QByteArray   & array, QString &dataOrigine, QString &dataURI, bool &dataGarbadgeable);

    void computeNameAndPlace(QStandardItem * item, const QString indexAsString, const QString &dataName, int &indexPlace, QString &tmpName);

    QString getPath(QModelIndex index = QModelIndex());
    QModelIndex getIndex(QString path);

    bool createFSDirectory(QString path, QString dirName);
    bool renameFSEntry    (QString path);
    bool removeFSDir      (QString path);
    bool removeFSFile     (QString path);
    bool moveFSEntry      (QString oldPath, QString newPath);

    bool writeCacheData(medAbstractData *  data, QString path, QString name);
    bool readCacheData (medAbstractData *& data, QString path, QString name);

    QModelIndex createFolderIndex(QStringList tree);

    void removeTooOldEntry();


public slots:
    void dataSaved(medDataIndex index); //remove a cache data and replace it by an uri on the source

    void addGeneratedData(medAbstractData * data, QString name = "");
    void addDataFromSource(medDataIndex index, medAbstractData * data = nullptr, const QModelIndex & parent = QModelIndex());
    void addDataFromFile(QString path, medAbstractData * data = nullptr, const QModelIndex & parent = QModelIndex());

    void pinData   (QModelIndex modelIndex);
    void unPinData (QModelIndex modelIndex);
    void removeData(QModelIndex modelIndex);

private slots:
    void renameByItem(QStandardItem * item);


signals:
    void editIndex(QModelIndex const &);
    void expandData(QModelIndex const &, bool);
    void visibility ();
    void visibled(bool state);


private:
    medVirtualRepresentationPrivate *d;
};

