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
//#include <QtSingleApplication>
#include <medDataIndex.h>
#include "QSingleApplication/qtlocalpeer.h"

class medApplicationPrivate;
class medMainWindow;

class medApplication : public QApplication //QtSingleApplication
{
    Q_OBJECT

public:
    medApplication(int & argc, char **argv);
    ~medApplication();

    bool event(QEvent *event) override;
    void setMainWindow(medMainWindow *mw);

signals:
    void showMessage(const QString& message);
    void messageReceived(const QString &message);

public slots:
    void redirectMessageToSplash(const QString& message);
    bool sendMessage(const QString &message, int timeout = 5000);

    void open(const medDataIndex & index);
    void open(QString path);

protected:
    void initialize();

private:
    medApplicationPrivate *d;
};
