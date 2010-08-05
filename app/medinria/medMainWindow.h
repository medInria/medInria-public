/* medMainWindow.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:47:51 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Jun 30 13:07:01 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 30
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDMAINWINDOW_H
#define MEDMAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QUrl>
class medDataIndex;
class medMainWindowPrivate;

class medMainWindow : public QMainWindow
{
    Q_OBJECT

public:
     medMainWindow(QWidget *parent = 0);
    ~medMainWindow(void);

    void readSettings(void);
    void writeSettings(void);

public slots:
    void setWallScreen(bool full);
    void setFullScreen(bool full);

    void switchToWelcomeArea(void);
    void switchToBrowserArea(void);
    void switchToViewerArea(void);
    void switchToDocumentationArea(void);

    void onConfigurationTriggered(QAction *action);

    void open(const medDataIndex& index);
    void open(const QString& file);

protected:
    void closeEvent(QCloseEvent *event);

private:
    medMainWindowPrivate *d;
};

#endif // MEDMAINWINDOW_H
