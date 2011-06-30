#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <medSql/medDatabaseController.h>
#include <medCore/medStorage.h>

#include <QtCore>

int medDatabaseControllerImplTest(int argc, char* argv[])
{
    // get temporary directory for new database
    QString tempPath = QDir::tempPath() + "/medinria-test-db";
    qDebug() << tempPath;

    medStorage::setDataLocation(tempPath);

    QPointer<medDatabaseControllerImpl> dbc = medDatabaseController::instance();

    if(!dbc->createConnection())
        qWarning() << "Unable to create a connection to the database" << endl;


    medStorage::removeDir(tempPath, NULL);

    medDatabaseController::destroy();

    return EXIT_FAILURE;
//    return EXIT_SUCCESS;
}
