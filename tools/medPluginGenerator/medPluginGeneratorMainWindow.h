/* medPluginGeneratorMainWindow.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 10 00:11:13 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Mar 16 09:45:03 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#pragma once

#include <QtGui>

class medPluginGeneratorMainWindowPrivate;

class medPluginGeneratorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
     medPluginGeneratorMainWindow(QWidget *parent = 0);
    ~medPluginGeneratorMainWindow();

public slots:
    void generate();

public slots:
    void about();
    void onOutputPathClicked();
    void onOutputPathChanged();
    void onPluginTypeChanged();
    void onNameChanged();
    void onDescriptionChanged();
    void onPluginLicenseChanged();
    void update();
    void onFamilyChanged(int);
    void onSetDefaultPath();
private:
    medPluginGeneratorMainWindowPrivate *d;
};


