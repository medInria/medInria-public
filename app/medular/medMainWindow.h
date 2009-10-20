/* medMainWindow.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:47:51 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct 20 09:56:50 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 8
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

class medMainWindowPrivate;
class QModelIndex;

class medMainWindow : public QMainWindow
{
    Q_OBJECT

public:
     medMainWindow(QWidget *parent = 0);
    ~medMainWindow(void);

    void readSettings(void);
    void writeSettings(void);

public slots:
    void switchToWelcomeArea(void);
    void switchToBrowserArea(void);
    void switchToViewerArea(void);

    void onPatientDoubleClicked(const QModelIndex &index);
    void onStudyDoubleClicked(const QModelIndex &index);
    void onSeriesDoubleClicked(const QModelIndex &index);

protected:
    void closeEvent(QCloseEvent *event);

private:
    medMainWindowPrivate *d;
};

#endif // MEDMAINWINDOW_H
