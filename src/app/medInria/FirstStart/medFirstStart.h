#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

/**
 * @file medFirstStart.h
 * @brief Header file for the medFirstStart class.
 * @copyright Copyright (c) INRIA 2013 - 2020. All rights reserved.
 * @see LICENSE.txt for details.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the  
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 */
#include <QObject>
#include <QString>
#include <QUrl>

#include <functional>

 /**
  * @brief Private class for medFirstStart.
  */
class medFirstStartPrivate;

class QNetworkAccessManager;

/**
 * @class medFirstStart
 * @brief This class provides functionalities to setup or update configuration files for another application.
 */
class medFirstStart : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Default constructor.
     */
    medFirstStart(QNetworkAccessManager *qnam);

    /**
     * @brief Destructor.
     */
    ~medFirstStart();

    /**
     * @brief Pushes a path to check for updates to the internal list.
     *
     * @param pathToCheck Path to the file to check for updates.
     * @param pathRessourceBase Base path for the resource.
     * @param featureName Name of the feature associated with the file.
     * @param fileName Name of the file.
     * @param url Optional URL for downloading new version for the reference file.
     * @param init Optional callback function to initialize the file.
     * @param update Optional callback function to update the file.
     */
    void pushPathToCheck(QString pathToCheck, QString pathRessourceBase,
                         QString featureName, //QString fileName,
                         QString url = "",
                         std::function<bool(QString, QString)> init = nullptr,
                         std::function<bool(QString, QString)> update = nullptr);

    /**
     * @brief Checks and updates all files added with pushPathToCheck.
     */
    void checkAndUpdate();

    void checks();
    void updates();

private:
    medFirstStartPrivate *d;
};

