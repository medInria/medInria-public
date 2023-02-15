#include "medVirtualRepresentation.h"

medVirtualRepresentation::medVirtualRepresentation(QObject *parent): m_basePath("", this)
{

    connect(&m_basePath, &medStringParameter::valueChanged,        this, &medVirtualRepresentation::fetch);
    connect(&m_watcher, SIGNAL(directoryChanged(const QString &)), this, SLOT(directoryChanged(QString const&)));
    connect(&m_watcher, SIGNAL(fileChanged(const QString &)),      this, SLOT(fileChanged(QString const&)));
}                       

medVirtualRepresentation::~medVirtualRepresentation()
{

}

bool medVirtualRepresentation::readFileEntry(QString filePath)
{
    return false;
}

bool medVirtualRepresentation::writeFileEntry(QJsonObject const & entry, QString filePath)
{
    return false;
}

bool medVirtualRepresentation::moveEntry(QString oldPath, QString newPath)
{
    return false;
}

bool medVirtualRepresentation::createDirectory()
{
    return false;
}

bool medVirtualRepresentation::renameFSEntry()
{
    return false;
}

bool medVirtualRepresentation::removeFile()
{
    return false;
}

bool medVirtualRepresentation::setRootPath(QString const & path) //TODO: to remove
{
    m_basePath.setValue(path);
    m_watcher.addPath(path);
    return fetch("");
}

bool medVirtualRepresentation::fetch(QString const & pi_path)
{
    bool bRes = true;

    QString path = m_basePath.value();
    if (!pi_path.isEmpty())
    {
        QFileInfo pathInfo(pi_path);
        if (pathInfo.isRelative())
        {
            path = QDir(path).filePath(pi_path);
        }
        else
        {
            path = pi_path;
        }
    }

    bool bOk = true;

    QDir dir(path);
    if (dir.exists())
    {
        dir.setFilter(QDir::NoSymLinks | QDir::NoDotAndDotDot);
        QDirIterator directory_walker(dir, QDirIterator::Subdirectories);
        QStringList files;



        files.clear();
        QDirIterator it(path, QStringList() << "*.json", QDir::NoFilter, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            files << it.next();
        }
        m_watcher.addPaths(files);
        files = files;



        files.clear();
        QDirIterator it2(path, QDir::NoDotAndDotDot | QDir::Dirs, QDirIterator::Subdirectories);
        while (it2.hasNext())
        {
            files << it2.next();
        }
        QStringList oldDirList_1 = m_watcher.directories();
        //m_watcher.addPaths(files);
        for (auto const & dirPathTmp : files)
        {
            bOk = m_watcher.addPath(dirPathTmp);
            if (bOk)
            {
                qDebug() << dirPathTmp;
            }
            else
            {
                qDebug() << dirPathTmp;
            }
        }
        QStringList oldDirList_2 = m_watcher.directories();
        files = files;
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

void medVirtualRepresentation::directoryChanged(QString const &path)
{
    bool bOk = true;
    QDir dirChanged(path);

    QStringList newDirList = dirChanged.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList oldDirList = m_watcher.directories();


    if (m_watcher.directories().contains(path))
    {
        if (dirChanged.exists())
        {
            bOk = fetch(path);
        }
        else
        {
            bOk = m_watcher.removePath(path);
        }
    }
    else
    {
        bOk = fetch(path);
    }
}

void medVirtualRepresentation::fileChanged(QString const & path)
{
    qDebug() << path;
}
