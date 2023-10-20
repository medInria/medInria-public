#pragma once
/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
 
#include <QtWidgets>
#include <QUuid>
#include <QUrl>
class medDataIndex;
class medMainWindowPrivate;

class medMainWindow : public QMainWindow
{
    Q_OBJECT

public:

     enum AreaType
     {
         HomePage,
         Browser,
         WorkSpace,
         Composer
     };

     medMainWindow(QWidget *parent = nullptr);
    ~medMainWindow();

    void restoreSettings();
    void saveSettings();
    QWidget* currentArea() const;
    QToolButton* notifButton();

    void setStartup(const AreaType areaIndex,const QStringList& filenames);

signals:
    void sliceSelected(int slice);
    void mainWindowActivated();
    void mainWindowDeactivated();
    void resized(QRect const &);

public slots:
    void setWallScreen(const bool full);
    void setFullScreen(const bool full);

    /**
     * @brief Switches from the Fullscreen mode to the normal mode.
     *
     */
    void toggleFullScreen();
    void switchToArea(const AreaType areaIndex);

    void open(const medDataIndex & index);
    void open(const QString& path);

    void processNewInstanceMessage(const QString& message);

    /**
     * @brief Enable or disable a menu in the application menu bar
     */   
    void enableMenuBarItem(QString name, bool enabled);

    /**
     * @brief Display the homepage area
     */
    void switchToHomepageArea();

    /**
     * @brief Display the search a toolbox window tool
     */
    void switchToSearchArea();

    /**
     * @brief In a view container, adjust the views sizes
     */
    void adjustContainersSize();

    /**
     * @brief Take a screenshot of the view
     */
    void captureScreenshot();

    /**
     * @brief Display the video export tool, to capture a movie of a view
     */
    void captureVideo();

    /**
     * @brief Switch to the default workspace saved in the settings
     */
    void switchToDefaultWorkSpace();

private slots:

    void showWorkspace(QString workspace);
    void showComposer();

    void switchToBrowserArea();
    void switchToWorkspaceArea();
    void switchToComposerArea();

    void showShortcutAccess();
    void hideShortcutAccess();

    void availableSpaceOnStatusBar();

    void open_waitForImportedSignal(medDataIndex,QUuid);

protected:
    void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent * event);
    bool event(QEvent * e);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    medMainWindowPrivate *d;
};
