#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QApplication>
#include <medDataIndex.h>
#include "QSingleApplication/qtlocalpeer.h"

class medApplicationPrivate;
class medMainWindow;

class medApplication : public QApplication
{
    Q_OBJECT

public:
    medApplication(int & argc, char **argv);
    ~medApplication();

    bool event(QEvent *event) override;
    bool notify(QObject *receiver, QEvent *e) override;
    void setMainWindow(medMainWindow *mw);
    void initializeSplashScreen();
    void initializeThemes();

signals:
    void showMessage(const QString& message);
    void messageReceived(const QString &message);

    void mouseGlobalClick(QPoint point);

public slots:
    bool sendMessage(const QString &message, int timeout = 5000);

    void open(const medDataIndex & index);
    void open(QString path);

    void listenClick(bool listen);

protected:
    void initialize();

private:
    medApplicationPrivate *d;
};
