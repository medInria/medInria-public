#pragma once
#include <QJsonObject>
#include <QStandardItemModel>

#include <medAbstractParameter.h>
#include <medAbstractData.h>
#include <medDataIndex.h>

/**
 * @brief Defines several constants used throughout the class.
 */
#define DATAORIGIN_ROLE 200  //String defined as below
#define DATAORIGIN_ROLE_FILESYSTEM "FS"
#define DATAORIGIN_ROLE_SOURCE     "source"
#define DATAORIGIN_ROLE_CACHE      "cache"

#define DATAURI_ROLE    201  //String URI of the data
#define DATAGARBAGEABLE_ROLE 202  //Boolean indicates if the data can be automatically removed (if longtime or too much data)

#define DATANAME_ROLE 300

class medVirtualRepresentationPrivate;

/**
 * @brief The medVirtualRepresentation class centralizes all data opened in medInria and also ensures the organization of data for the user.
 */
class MEDCORE_EXPORT medVirtualRepresentation : public QStandardItemModel
{
    Q_OBJECT
public:
    medVirtualRepresentation(QObject *parent = nullptr);
    ~medVirtualRepresentation();


    bool remove(QModelIndex index);
    bool rename(QModelIndex index, QString newName);
    bool create(QModelIndex parent, QString dirName);

    
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    QMimeData *	mimeData(const QModelIndexList &indexes) const override;
    QStringList	mimeTypes() const override;
    Qt::DropActions	supportedDropActions() const override;
    
    QModelIndex getModelIndex(medDataIndex index);
    QList<medAbstractParameter*> getParams();


public slots:
    void addGeneratedData(medAbstractData * data, QString name = "");
    void addData(medDataIndex medIndex, QString volumeName = "", medAbstractData * data = nullptr, const QModelIndex & parent = QModelIndex());

    void pinData   (QModelIndex modelIndex);
    void unPinData (QModelIndex modelIndex);
    void removeData(QModelIndex modelIndex);

    void dataSaved(medDataIndex index); //remove a cache data and replace it by an uri on the source

private:
    // JSon handling section
    bool fetch(QString const &path = QString());
    bool readFileEntry(QString filePath, QString &dataOrigine, QString &dataURI, bool &dataGarbadgeable);
    bool writeFileEntry(QJsonObject const & entry, QString filePath, QString fileName);
    bool writeJson(const QModelIndex  & index, QJsonObject &json);
    bool readJson (const QByteArray   & array, QString &dataOrigine, QString &dataURI, bool &dataGarbadgeable);


    // File system handling for json section
    bool createFSDirectory(QString path, QString dirName);
    bool renameFSEntry    (QString path);
    bool removeFSDir      (QString path);
    bool removeFSFile     (QString path);
    bool moveFSEntry      (QString oldPath, QString newPath);
    
    // Cache section
    bool writeCacheData(medAbstractData *  data, QString path, QString name);
    bool readCacheData (medAbstractData *& data, QString path, QString name);

    // Internal routines section
    void computeNameAndPlace(QStandardItem * item, const QString indexAsString, const QString &dataName, int &indexPlace, QString &tmpName);
    void insertData(medAbstractData * data, QString &uri, QString &dataPath, QString &tmpName);
    QModelIndex createFolderIndex(QStringList tree);
    QString getPath(QModelIndex index = QModelIndex());
    QModelIndex getIndex(QString jsonPath);

    void removeTooOldEntry();

private slots:
    void renameByItem(QStandardItem * item);

signals:
    /**
     * @brief Signal emitted when an index is edited.
     * @param index The edited index.
     */
    void editIndex(QModelIndex const &);

    /**
     * @brief Signal emitted when data is expanded.
     * @param index The index of the expanded data.
     * @param state The expansion state (true for expanded, false for collapsed).
     */
    void expandData(QModelIndex const &, bool);
    
    /**
     * @brief Signal emitted when the visibility changes.
     */
    void visibility();
    
    /**
     * @brief Signal emitted when the visibility state changes.
     * @param state The new visibility state.
     */
    void visibled(bool state);


private:
    medVirtualRepresentationPrivate *d;
};

