/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

// Must be included first (see the header notes for details)
#include "medPythonCoreAPI.h"

#include "medPythonCoreInit.h"

#include <QDebug>
#include <QFileInfo>
#include <QTemporaryDir>

#include <medExternalResources.h>

#include "medPythonCoreUtils.h"

namespace med::python
{

namespace
{

bool getResourcesDirectory(QString& resourcesDirectory)
{
    static QString staticResourcesDirectory = med::getExternalResourcesDirectory(TARGET_NAME);
    bool success = !staticResourcesDirectory.isEmpty();

    if (success)
    {
        resourcesDirectory = staticResourcesDirectory;
    }
    else
    {
        qWarning() << "Cannot find the resources directory for " << TARGET_NAME;
    }

    return success;
}

bool getResourceArchives(QStringList& resourceArchives)
{
    QString resourcesDirectory;
    bool success = getResourcesDirectory(resourcesDirectory);

    if (success)
    {
        QFileInfoList fileInfoList = QDir(resourcesDirectory).entryInfoList({"*.zip"});
        resourceArchives.clear();

        foreach (QFileInfo fileInfo, fileInfoList)
        {
            resourceArchives << fileInfo.absoluteFilePath();
        }
    }

    return success;
}

// The encodings module is required during initialiation, and cannot be
// contained within a ZIP archive (Python loads it before loading zlib). The
// module is a package but the resource system does not allow storing
// subdirectories. To solve this problem, the package files are stored in the
// resource directory with the "encodings_" prefix added to them. At runtime, an
// encodings directory is created within a temporary directory, with symbolic
// links to the actual encodings files. The temporary directory is then added to
// Python's module paths.
//
QScopedPointer<QTemporaryDir> temporaryEncodingsParentDirectory;

bool getEncodingsParentDirectory(QString& parentDirectory)
{
    bool success = true;

    if (temporaryEncodingsParentDirectory.isNull())
    {
        temporaryEncodingsParentDirectory.reset(new QTemporaryDir);
        success = temporaryEncodingsParentDirectory->isValid();

        if (!success)
        {
            qWarning() << "Cannot create temporary directory for the encodings package: " << temporaryEncodingsParentDirectory->errorString();
        }
    }

    parentDirectory = temporaryEncodingsParentDirectory->path();
    return success;
}

bool getEncodingsDirectory(QString& encodingsDirectory)
{
    static QString staticEncodingsDirectory;
    bool success = true;

    if (staticEncodingsDirectory.isEmpty())
    {
        QString parentDirectory;
        success = getEncodingsParentDirectory(parentDirectory);

        if (success)
        {
            QDir workingDirectory(parentDirectory);
            success = workingDirectory.mkpath("encodings")
                      && workingDirectory.cd("encodings");

            if (success)
            {
                staticEncodingsDirectory = workingDirectory.absolutePath();
            }
            else
            {
                qWarning() << "Cannot create encodings package directory";
            }
        }
    }

    encodingsDirectory = staticEncodingsDirectory;
    return success;
}

bool getEncodingsFiles(QFileInfoList& encodingsFiles)
{
    QString resourcesDirectory;
    bool success = getResourcesDirectory(resourcesDirectory);

    if (success)
    {
        encodingsFiles = QDir(resourcesDirectory).entryInfoList({"encodings_*.py"});
    }

    return success;
}

bool createEncodingsLink(QFileInfo encodingsFile)
{
    QString encodingsDirectory;
    bool success = getEncodingsDirectory(encodingsDirectory);

    if (success)
    {
        QString linkName = encodingsFile.fileName().remove(QRegExp("encodings_"));
        QString linkPath = QDir(encodingsDirectory).absoluteFilePath(linkName);
        QString filePath = encodingsFile.absoluteFilePath();
        success = QFile::link(filePath, linkPath);

        if (!success)
        {
            qWarning() << "Cannot create symbolic link " << filePath << " -> " << linkPath;
        }
    }

    return success;
}

bool setupEncodingsPackage()
{
    QFileInfoList encodingsFiles;
    bool success = getEncodingsFiles(encodingsFiles);

    if (success)
    {
        foreach (QFileInfo file, encodingsFiles)
        {
            success &= createEncodingsLink(file);
        }
    }

    return success;
}

bool getCoreModulePaths(QStringList& modulePaths)
{
    QString resourcesDirectory;
    QStringList resourceArchives;
    QString encodingsParentDirectory;
    bool success = getResourcesDirectory(resourcesDirectory)
                   && getResourceArchives(resourceArchives)
                   && getEncodingsParentDirectory(encodingsParentDirectory);

    if (success)
    {
        modulePaths.clear();
        modulePaths << resourcesDirectory << resourceArchives << encodingsParentDirectory;
    }

    return success;
}

bool checkStatus(PyStatus status)
{
    bool success = !PyStatus_Exception(status);

    if (!success)
    {
        qWarning() << "Error during initialization of Python: " << status.err_msg;
    }

    return success;
}

bool preInitialize()
{
    PyPreConfig preConfig;
    PyPreConfig_InitIsolatedConfig(&preConfig);
    preConfig.utf8_mode = 1;
    return checkStatus(Py_PreInitialize(&preConfig));
}

bool setConfigOptions(PyConfig* config, QStringList modulePaths)
{
    wchar_t** moduleSearchPaths = qStringListToWideChar(modulePaths);
    config->module_search_paths_set = 1;
    config->write_bytecode = 0;

    // Even if empty, every one of the options below must be set to prevent recalculation of the paths
    //
    bool success = checkStatus(PyConfig_SetBytesString(config, &config->base_executable, TARGET_NAME))
                   && checkStatus(PyConfig_SetBytesString(config, &config->executable, TARGET_NAME))
                   && checkStatus(PyConfig_SetBytesString(config, &config->prefix, qUtf8Printable("")))
                   && checkStatus(PyConfig_SetBytesString(config, &config->base_exec_prefix, qUtf8Printable("")))
                   && checkStatus(PyConfig_SetBytesString(config, &config->base_prefix, qUtf8Printable("")))
                   && checkStatus(PyConfig_SetBytesString(config, &config->exec_prefix, qUtf8Printable("")))
                   && checkStatus(PyConfig_SetWideStringList(config, &config->module_search_paths, modulePaths.size(), moduleSearchPaths));

    freeWideCharList(moduleSearchPaths, modulePaths.size());
    return success;
}

bool prepareConfig(PyConfig* config, QStringList additionalModulePaths)
{
    QStringList coreModulePaths;
    bool success = getCoreModulePaths(coreModulePaths);

    if (success)
    {
        PyConfig_InitIsolatedConfig(config);
        success = setConfigOptions(config, coreModulePaths + additionalModulePaths);
    }

    return success;
}

bool initializeFromConfig(PyConfig* config)
{
    bool success = checkStatus(Py_InitializeFromConfig(config));

    if (success)
    {
        PyConfig_Clear(config);
    }

    return success;
}

} // namespace

bool initializeInterpreter(QStringList additionalModulePaths)
{
    PyConfig config;
    bool success = setupEncodingsPackage()
                   && preInitialize()
                   && prepareConfig(&config, additionalModulePaths)
                   && initializeFromConfig(&config);

    if (success)
    {
        qInfo() << "Embedded Python initialized: " << Py_GetVersion();
    }
    else
    {
        qWarning() << "Initialization of the embedded Python failed";
        finalizeInterpreter();
    }

    return success;
}

bool finalizeInterpreter()
{
    bool success = true;

    if (Py_IsInitialized())
    {
        success = (Py_FinalizeEx() == 0);
    }

    return success;
}

QStringList test::getTemporaryDirectories()
{
    return {temporaryEncodingsParentDirectory->path()};
}

} // namespace med::python
