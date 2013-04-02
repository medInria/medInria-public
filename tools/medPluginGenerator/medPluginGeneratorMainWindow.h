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
    ~medPluginGeneratorMainWindow(void);

public slots:
    void generate(void);

public slots:
    void about(void);
    void onOutputPathClicked(void);
    void onOutputPathChanged(void);
    void onPluginTypeChanged(void);
    void onNameChanged(void);
    void onDescriptionChanged();
    void onPluginLicenseChanged();
    void update(void);
    void onFamilyChanged(int);
    void onSetDefaultPath();
private:
    medPluginGeneratorMainWindowPrivate *d;
};


