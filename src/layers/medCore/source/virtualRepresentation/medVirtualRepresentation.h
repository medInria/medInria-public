#pragma once
#include <QFileSystemWatcher>
#include <QJsonObject>

#include <medStringParameter.h>
#include <medDataIndex.h>

class medVirtualRepresentation : public QObject
{
    Q_OBJECT
public:
    medVirtualRepresentation(QObject *parent = nullptr);
    ~medVirtualRepresentation();

    bool readFileEntry(QString filePath);
    bool writeFileEntry(QJsonObject const & entry, QString filePath);
    bool moveEntry(QString oldPath, QString newPath);

    bool createDirectory();
    bool renameFSEntry();
    bool removeFile();

    bool setRootPath(QString const &path);

private:
    bool fetch(QString const &path = QString());

public slots:
    void directoryChanged(QString const &path);
    void fileChanged(QString const &path);

private:
    medStringParameter m_basePath;
    QFileSystemWatcher m_watcher;
    QMap<QString, QPair<QStringList, QStringList> > m_flatTree; //Path, Dirs list, files list
};