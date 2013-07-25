/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

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
    ~medMainWindow();

    void readSettings();
    void writeSettings();

    void setStartup(const AreaType areaIndex,const QStringList& filenames);
    void resizeEvent( QResizeEvent * event );

signals:
    void sliceSelected(int slice);

public slots:
    void setWallScreen(const bool full);
    void setFullScreen(const bool full);

    /**
     * @brief Switches from the Fullscreen mode to the normal mode.
     *
     */
    void switchFullScreen();
    void switchToArea(const AreaType areaIndex);

private slots:

    void captureScreenshot();

    void switchToBrowserArea();
    void switchToWorkspaceArea();
    void switchToHomepageArea();

    void showWorkspace(QString workspace);
    void showQuickAccess();
    void hideQuickAccess();
    void showShortcutAccess();
    void hideShortcutAccess();
    void onWorkspaceTriggered(QAction *action);
    void onEditSettings();

    void availableSpaceOnStatusBar();
    //  @brief React to to new medInria instance.
    //  @param message

    void onNewInstance(const QString& message);

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
    * Opens an image (which is already in the database)
    * in the viewer area (when the user double-clicks on a thumbnail)
    * @param index - the @medDataIndex of the image
    * @param slice - the slice number corresponding to the image
    **/
    void open(const medDataIndex& index, int slice);

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
    
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    
    int saveModified();

    bool eventFilter(QObject * obj, QEvent *ev);

    /**
     * all initial registering should be done here
     */
    void registerToFactories();

private:
    medMainWindowPrivate *d;
};


