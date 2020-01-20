#ifndef MEDBOUTIQUESCONFIGURATION_H
#define MEDBOUTIQUESCONFIGURATION_H

#include <QtGlobal>
#include <QCoreApplication>
#include <QString>
#include <QDir>

// #define BOUTIQUE_GUI_STANDALONE
// #define TEST_DRAGGABLE_INPUTS

#ifdef BOUTIQUE_GUI_STANDALONE
#define BOUTIQUES_DIRECTORY "/../BoutiquesGUI-Data/"
#else
#define BOUTIQUES_DIRECTORY "/BoutiquesGUI-Data/"
#endif


#ifndef Q_OS_WIN
#define PYTHON_PATH "python3"
#else
#define PYTHON_PATH "python/python.exe"
#endif

#define BOUTIQUES_GUI_SETTINGS_PATH "boutiques-gui-settings.json"
#define BOUTIQUES_TEMPORARY_DIR "/TemporaryFiles/"
#define DOCKER_PATH "docker"
#define BOSH_PATH "boutiques/bosh.py"
#define VCREDIS_PATH "vc_redist.x86.exe"

class BoutiquesPaths {
public:
    static QString Boutiques()
    {
        return QDir(QCoreApplication::applicationDirPath() + BOUTIQUES_DIRECTORY).absolutePath();
    }

    static QDir BoutiquesTemp()
    {
        QDir temporaryDirectory(QCoreApplication::applicationDirPath() + BOUTIQUES_DIRECTORY + BOUTIQUES_TEMPORARY_DIR);
        if (!temporaryDirectory.exists()){
          temporaryDirectory.mkpath(".");
        }
        return temporaryDirectory;
    }

    static QString Settings()
    {
        return QFileInfo(QCoreApplication::applicationDirPath() + BOUTIQUES_DIRECTORY + BOUTIQUES_GUI_SETTINGS_PATH).absoluteFilePath();
    }

    static QString Python()
    {
#ifndef Q_OS_WIN
        return PYTHON_PATH;
#else
        return "\"" + QFileInfo(QCoreApplication::applicationDirPath() + BOUTIQUES_DIRECTORY + PYTHON_PATH).absoluteFilePath() + "\"";
#endif
    }
    static QString Bosh()
    {
        return QFileInfo(QCoreApplication::applicationDirPath() + BOUTIQUES_DIRECTORY + BOSH_PATH).absoluteFilePath();
    }

    static QString Docker()
    {
        return DOCKER_PATH;
    }

    static QString VCRedis()
    {
        return QFileInfo(QCoreApplication::applicationDirPath() + BOUTIQUES_DIRECTORY + VCREDIS_PATH).absoluteFilePath();
    }
};

#endif // MEDBOUTIQUESCONFIGURATION_H
