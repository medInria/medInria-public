/* medMainWindow.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:47:51 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Mar 19 16:28:20 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 21
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
    void setFullScreen(bool full);

    void switchToWelcomeArea(void);
    void switchToBrowserArea(void);
    void switchToViewerArea(void);
    void switchToDocumentationArea(void);

    void onPatientDoubleClicked(const QModelIndex &index);
    void onStudyDoubleClicked(const QModelIndex &index);
    void onSeriesDoubleClicked(const QModelIndex &index);

protected:
    void closeEvent(QCloseEvent *event);

private:
    medMainWindowPrivate *d;
};

#endif // MEDMAINWINDOW_H
