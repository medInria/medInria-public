/* medMainWindow.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:47:51 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  7 12:31:41 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 32
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
#include <QUuid>
#include <QUrl>
class medDataIndex;
class medMainWindowPrivate;

class medMainWindow : public QMainWindow
{
    Q_OBJECT

public:

     typedef enum { HomePage, Browser, WorkSpace } AreaType;

     medMainWindow(QWidget *parent = 0);
    ~medMainWindow(void);

    void readSettings(void);
    void writeSettings();

    void setStartup(const AreaType areaIndex,const QString& filename);
    void updateQuickAccessMenu(void);
    void resizeEvent( QResizeEvent * event );

public slots:
    void setWallScreen(const bool full);
    void setFullScreen(const bool full);

    /**
     * @brief Switches from the Fullscreen mode to the normal mode.
     *
     */
    void switchFullScreen(void);
    void switchToArea(const AreaType areaIndex);

private slots:

    void switchToBrowserArea(void);
    void switchToWorkspaceArea(void);
    void switchToHomepageArea(void);

    void onShowWorkspace(QString workspace);
    void onShowQuickAccess(void);
    void onHideQuickAccess(void);
    void onWorkspaceTriggered(QAction *action);
    void onQuit(void);
    void onSaveModified(void);
    void onNoQuit(void);
    void onEditSettings(void);

    /**
     * @brief: Decides what to do when the importer returned a medDataIndex after importation.
     *
     * @param index the medDataIndex returned by the importer. May be invalid if iportation was not successful.
     * @param importUuid This string is the representation of a QUuid.
     * If the QUuid identifies a request made by this object, then the action is triggered. Here we call openInTab in the workspaceArea.
     */
    void onOpenFile(const medDataIndex & index,const QString& importUuid);

    /**
    * Opens an image (which is already in the database)
    * in the viewer area.
    * @param index - the @medDataIndex of the image
    **/
    void open(const medDataIndex& index);

    /**
    * Imports the image/s in the non-persistent database
    * and the opens it in the viewer area and switches to it.
    * @param path - the path to the file or directory
    **/
    void open(const QString& path);

    /**
    * Imports the image/s in the non-persistent database
    * but does not switch to the viewer area.
    * @param path - the path to the file or directory
    **/
    void load(const QString& path);

    /**
     * @brief Overload existing showFullScreen().
     *
     * Allows the update of the fullScreen button.
     */
    void showFullScreen();

    /**
     * @brief Overload existing showNormal().
     *
     * Allows the update of the fullScreen button.
     */
    void showNormal();

    /**
     * @brief Overload existing showMaximized().
     *
     * Allows the update of the fullScreen button.
     */
    void showMaximized();

protected:
    void closeEvent(QCloseEvent *event);

    void mousePressEvent(QMouseEvent * event);

    /**
     * all initial registering should be done here
     */
    void registerToFactories();

private:
    medMainWindowPrivate *d;
};

#endif // MEDMAINWINDOW_H
