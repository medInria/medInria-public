/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medApplication.h"

#include <locale.h>

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
    medMainWindow *mainWindow;
    QStringList systemOpenInstructions;

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

        const QString& newUserPath = userPath+organization+"/medInria.conf";

        if (!QFile::exists(newUserPath))
        {
            const QString& oldUserPath = userPath+organization+"/medinria.conf";

            if (QFile::exists(oldUserPath) )
            {
                qWarning() << "Updating old medinria.conf file";
                QFile oldSettings (oldUserPath);
                const bool result = oldSettings.rename(userPath+organization+QLatin1Char('/')+
                                    applicationName+".conf" );
                if (!result)
                {
                    qWarning() << "Could not rename medinria.conf\n"
                               << "Either you don't have write access\n"
                               << "or medInria.conf already exists.";
                }
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
        QtSingleApplication(argc,argv),
        d(new medApplicationPrivate)
{
    d->mainWindow = NULL;

    this->setApplicationName("medInria");
    qDebug() << "Version:" << MEDINRIA_VERSION;
    this->setApplicationVersion(MEDINRIA_VERSION);
    this->setOrganizationName("inria");
    this->setOrganizationDomain("fr");
    this->setWindowIcon(QIcon(":/medInria.ico"));

    d->fixSettingsPath(this->organizationName(),this->applicationName());

    qDebug() <<  "default data location:" << QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    medStyleSheetParser parser(dtkReadFile(":/medInria.qss"));
    this->setStyleSheet(parser.result());

    //  Set some splash screen properties:

    setMsgColor(Qt::black);
    setMsgAlignment(Qt::AlignLeft);

    //  Redirect msgs to the logs

    QObject::connect(medPluginManager::instance(), SIGNAL(loadError(const QString &)),
                     this, SLOT(redirectErrorMessageToLog(const QString&)) );
    QObject::connect(medPluginManager::instance(), SIGNAL(loaded(QString)),
                     this, SLOT(redirectMessageToLog(QString)) );

    QObject::connect(this,SIGNAL(messageReceived(const QString&)),
                     this,SLOT(redirectMessageToLog(QString)));
}

medApplication::~medApplication(void)
{
    delete d;
    d = NULL;
}

bool medApplication::event(QEvent *event)
{
    switch (event->type())
    {
        // Handle file system open requests, but only if the main window has been created and set
        case QEvent::FileOpen:
            if (d->mainWindow)
                emit messageReceived(QString("/open ") + static_cast<QFileOpenEvent *>(event)->file());
            else
                d->systemOpenInstructions.append(QString("/open ") + static_cast<QFileOpenEvent *>(event)->file());
            return true;
        default:
            return QtSingleApplication::event(event);
    }
}

void medApplication::setMainWindow(medMainWindow *mw)
{
    d->mainWindow = mw;

    // If there are any requests to open files not yet treated, send signal to do so
    foreach(QString openInstruction, d->systemOpenInstructions)
    {
        emit messageReceived(openInstruction);
    }

    d->systemOpenInstructions.clear();
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
