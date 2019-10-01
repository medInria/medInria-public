/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

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

     medMainWindow(QWidget *parent = 0);
    ~medMainWindow();

    void restoreSettings();
    void saveSettings();
    QWidget* currentArea() const;

    void setStartup(const AreaType areaIndex,const QStringList& filenames);
    void resizeEvent( QResizeEvent * event );

signals:
    void sliceSelected(int slice);
    void mainWindowActivated();
    void mainWindowDeactivated();


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

private slots:

    void showWorkspace(QString workspace);
    void showComposer();

    void captureScreenshot();

    void switchToBrowserArea();
    void switchToSearchArea();
    void switchToWorkspaceArea();
    void switchToHomepageArea();
    void switchToComposerArea();

    void toggleQuickAccessVisibility();
    void hideQuickAccess();
    void showShortcutAccess();
    void hideShortcutAccess();

    void availableSpaceOnStatusBar();

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

    void adjustContainersSize();

    void open_waitForImportedSignal(medDataIndex,QUuid);

protected:
    void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent * event);
    int saveModifiedAndOrValidateClosing();
    bool event(QEvent * e);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    medMainWindowPrivate *d;
};


