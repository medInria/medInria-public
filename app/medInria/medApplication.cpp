/* medApplication.cpp ---
 *
 * Author: John Stark
 * Copyright (C) 2011 - John Stark, Inria.
 * Created: May 2011
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medApplication.h"

#include <QtGui>

#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>
#include <medPluginManager.h>

class medApplicationPrivate
{
public:
    QColor msgColor;
    int msgAlignment;

    /*
      fix the settings filename in the move 2.0.0 -> 2.0.1
    */
    inline void fixSettingsPath(const QString & organization,
                           const QString & applicationName)
    {
    #ifdef Q_OS_WIN
        //maybe fix registry keys???
    #else
    #ifdef Q_OS_LINUX

        //Takes code from qsettings.cpp to get the default place for settings
        QString userPath;
        QString homePath = QDir::homePath();
        char *env = getenv("XDG_CONFIG_HOME");
        if (env == 0)
        {
            userPath = homePath;
            userPath += QLatin1Char('/');
    #ifdef Q_WS_QWS
            userPath += QLatin1String("Settings");
    #else
            userPath += QLatin1String(".config");
    #endif
        } else if (*env == '/')
        {
            userPath = QLatin1String(env);
        } else {
            userPath = homePath;
            userPath += QLatin1Char('/');
            userPath += QLatin1String(env);
        }
        userPath += QLatin1Char('/');

        QString oldUserPath= userPath + organization +
                "/medinria.conf";//app.applicationName();

        if (QFile::exists(oldUserPath) )
        {
            qWarning() << "Updating old medinria.conf file";
            QFile oldSettings (oldUserPath);
            bool result = oldSettings.rename(userPath + organization +
                                             QLatin1Char('/') +
                               applicationName + ".conf" );
            if (!result)
            {
                qWarning() << "Could not rename medinria.conf\n"
                           << "Either you don't have write access\n"
                           << "or medInria.conf already exists.";
            }
        }
    #endif //Q_OS_LINUX
    #endif //Q_OS_WIN
    }
};

// /////////////////////////////////////////////////////////////////
// medApplication
// /////////////////////////////////////////////////////////////////

medApplication::medApplication(int & argc, char**argv) :
        QApplication(argc,argv),
        d(new medApplicationPrivate)
{
    this->setApplicationName("medInria");
    qDebug() << "Version:" << MEDINRIA_VERSION;
    this->setApplicationVersion(MEDINRIA_VERSION);
    this->setOrganizationName("inria");
    this->setOrganizationDomain("fr");
    this->setWindowIcon(QIcon(":/medInria.ico"));

    d->fixSettingsPath(this->organizationName(),this->applicationName());

    qDebug() <<  "default data location:" << QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    //The ownership of the style object is transferred.
    this->setStyle( new QPlastiqueStyle() );
    this->setStyleSheet(dtkReadFile(":/medInria.qss"));

    //Set some splash screen properties:
    setMsgColor(Qt::black);
    setMsgAlignment(Qt::AlignLeft);

    //redirect medPluginManager msgs to the logs
    QObject::connect(medPluginManager::instance(), SIGNAL(loadError(const QString &)),
                     this, SLOT(redirectErrorMessageToLog(const QString&)) );
    QObject::connect(medPluginManager::instance(), SIGNAL(loaded(QString)),
                     this, SLOT(redirectMessageToLog(QString)) );
}

medApplication::~medApplication(void)
{
    delete d;
    d = NULL;
}

void medApplication::setMsgColor(const QColor& color)
{
    d->msgColor = color;
}

const QColor& medApplication::msgColor()
{
    return d->msgColor;
}

int medApplication::msgAlignment()
{
    return d->msgAlignment;
}

void medApplication::setMsgAlignment(int alignment)
{
    d->msgAlignment = alignment;
}

void medApplication::redirectMessageToLog(const QString &message)
{
    dtkTrace()<< message;
}

void medApplication::redirectErrorMessageToLog(const QString &message)
{
    dtkError()<< message;
}

void medApplication::redirectMessageToSplash(const QString &message)
{
    emit showMessage(message,d->msgAlignment,d->msgColor);
}
