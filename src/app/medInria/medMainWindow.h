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
         WorkSpace
     };

     medMainWindow(QWidget *parent = nullptr);
    ~medMainWindow();

     void initMenuBar(QWidget * parent);
     void menuNotif(QMenuBar * menu_bar);
     void menuAbout(QMenuBar * menu_bar);
     void menuSettings(QMenuBar * menu_bar);
     void menuCapture(QMenuBar * menu_bar);
     void menuWindow(QMenuBar * menu_bar);
     void menuWorkspace(QMenuBar * menu_bar);
     void menuFile(QMenuBar * menu_bar);

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


    /**
     * @brief Switch visibility of notification panel
     */
    void toggleNotificationPanel();

private slots:

    void showWorkspace(QString workspace);

    void switchToBrowserArea();
    void switchToWorkspaceArea();

    void showShortcutAccess();
    void hideShortcutAccess();

    //void availableSpaceOnStatusBar();

    void open_waitForImportedSignal(medDataIndex,QUuid);


    void openFromSystem();
    void openDicomFromSystem();

    void onShowBrowser();
    void onShowDataSources();
    //void onShowNotifPanel();
    void onShowHelp();

    /**
     * @brief Display an About window
     */
    void onShowAbout();

    /**
     * @brief Display a window with the authors list
     */
    void onShowAuthors();

    /**
     * @brief Display a window with the release notes
     */
    void onShowReleaseNotes();

    /**
     * @brief Display a license window
     */
    void onShowLicense();

    /**
     * @brief Display a database window
     */
     // void onShowDatabase();

     /**
      * @brief Display a settings window for the Area section
      */
    void onShowAreaSettings();

    /**
     * @brief Switch the application to the asked workspace
     */
    void onShowWorkspace(QString workspace);

    /**
     * @brief Switch the application to the current workspace name
     */
    void onSwitchToWorkspace();

    /**
     * @brief Open the log directory
     */
    void openLogDirectory();

    /**
     * @brief Display a window with the plugins logs from the application start
     */
    void onShowPluginLogs();

    /**
     * @brief Update the fullscreen icon if checked or not
     */
    void switchOffOnFullscreenIcons(const bool checked);


    void filterWSMenu(QString text);


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
