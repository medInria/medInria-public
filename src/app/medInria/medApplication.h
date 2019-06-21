/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once
#include <QtSingleApplication>
#include <medDataIndex.h>

class medApplicationPrivate;
class medMainWindow;

class medApplication : public QtSingleApplication
{
    Q_OBJECT

public:
    medApplication(int & argc, char **argv);
    ~medApplication();

    bool event(QEvent *event);
    void setMainWindow(medMainWindow *mw);

signals:
    void showMessage(const QString& message);

public slots:
    void redirectMessageToSplash(const QString& message);

    void open(const medDataIndex & index);
    void open(QString path);

protected:
    void initialize();

private:
    medApplicationPrivate *d;
};


