#ifndef MEDBOUTIQUESINSTALLER_H
#define MEDBOUTIQUESINSTALLER_H

#include "medBoutiquesConfiguration.h"
#include <QWidget>

namespace medBoutiquesInstaller
{
    // Check the "installed" setting: perform install checks if false, ignore install otherwise
    void checkBoutiquesInstallation(QWidget *parent);

    // Perform installation check: verify that python and docker can run properly and warn the user otherwise
    // Set the "installed" setting to "true" if everything works fine
    void installBoutiques(QWidget *parent, QJsonObject *settings = nullptr);

    // Set the "installed" setting to "true"
    void setBoutiquesInstalled(QJsonObject *settings);

    // Check that the python command works ("python" on Linux and OS X, "$EXEC_PATH/BoutiquesGUI-DATA/python/python.exe" on Windows)
    bool isPythonWorking(QWidget *parent, const QString &version = "");

    // Check that the docker command works "docker"
    bool isDockerWorking(QWidget *parent);
};

#endif // MEDBOUTIQUESINSTALLER_H
