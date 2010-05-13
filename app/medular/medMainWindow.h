/* medMainWindow.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:47:51 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 16:35:43 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 23
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

class QModelIndex;

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
    void showInterpreter(void);
    void setWallScreen(bool full);
    void setFullScreen(bool full);

    void switchToWelcomeArea(void);
    void switchToBrowserArea(void);
    void switchToViewerArea(void);
    void switchToDocumentationArea(void);

    void onPatientDoubleClicked(const QModelIndex &index);
    void onStudyDoubleClicked(const QModelIndex &index);
    void onSeriesDoubleClicked(const QModelIndex &index);

    void open(const QString& file);

protected:
    void closeEvent(QCloseEvent *event);

private:
    medMainWindowPrivate *d;
};

#endif // MEDMAINWINDOW_H
