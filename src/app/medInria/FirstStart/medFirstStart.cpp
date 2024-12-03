/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

/**
 * @file medFirstStart.cpp
 * @brief Implementation file for the medFirstStart class.
 * @copyright Copyright (c) INRIA 2013 - 2020. All rights reserved.
 * @see LICENSE.txt for details.
 */
#include "medFirstStart.h"

#include "medFirstStartClient.h"
#include "medFirstStartUpdateFile.h"

#include <QFileInfo>
#include <QRunnable>
#include <QThreadPool>
#include <QVector>

class medFirstStartPrivate
{
public:
    medFirstStartPrivate() = default;
    ~medFirstStartPrivate() = default;

    /**
     * @struct st_fileInfoUpdate
     * @brief Internal structure to store information about a file to check and update.
     */
    struct st_fileInfoUpdate
    {
        QString pathToCheck;
        QString pathRessourceBase;
        QString featureName;
        QString fileName;
        QString urlDL;
        std::function<bool(QString, QString)> init;
        std::function<bool(QString, QString)> update;
    };

    /**
     * @brief List of files to check install or update.
     */
    QVector<struct st_fileInfoUpdate> listFilesToCheckAndUpdate;
    QNetworkAccessManager *qnam;
    
};

class medUpdateReferencesFilesThread : public QRunnable
{
public:
    medUpdateReferencesFilesThread(medFirstStart * parent) : m_parent(parent) {};
    virtual ~medUpdateReferencesFilesThread() = default;

    void run() override
    {
        m_parent->updates();
    }

    medFirstStart * m_parent;
};

medFirstStart::medFirstStart(QNetworkAccessManager *qnam)
{
    d = new medFirstStartPrivate();
    d->qnam = qnam;
}

medFirstStart::~medFirstStart()
{
    delete d;
}




void medFirstStart::pushPathToCheck(QString pathToCheck, QString pathRessourceBase, QString featureName,  QString url, std::function<bool(QString, QString)> init, std::function<bool(QString, QString)> update)
{
    d->listFilesToCheckAndUpdate.push_back({ pathToCheck, pathRessourceBase, featureName, QFileInfo(pathToCheck).fileName(), url, init, update });
}

void medFirstStart::checkAndUpdate()
{
    checks();
    medUpdateReferencesFilesThread * updateThread = new medUpdateReferencesFilesThread(this);
    QThreadPool::globalInstance()->start(updateThread);
}

void medFirstStart::checks()
{
    for (auto & fileToCheckAndUpdate : d->listFilesToCheckAndUpdate)
    {
        medFirstStartUpdateFile fileUpdater(fileToCheckAndUpdate.pathToCheck, fileToCheckAndUpdate.featureName, fileToCheckAndUpdate.pathRessourceBase, fileToCheckAndUpdate.update, fileToCheckAndUpdate.init);
        fileUpdater.installFile();
    }
}

void medFirstStart::updates()
{
    for (auto & fileToCheckAndUpdate : d->listFilesToCheckAndUpdate)
    {
        medFirstStartClient fileClientDL(fileToCheckAndUpdate.featureName, fileToCheckAndUpdate.fileName, fileToCheckAndUpdate.urlDL, d->qnam);
        fileClientDL.updateReferenceFile();
    }
}
