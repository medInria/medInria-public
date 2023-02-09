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

#pragma once

#include <QtGui>
#include <QtWidgets>

class medHomepageAreaPrivate;

class medHomepageArea : public QWidget
{
Q_OBJECT
public:
    medHomepageArea(QWidget * parent = nullptr);
    virtual ~medHomepageArea();

    /**
     * @brief Display the workspace buttons on homepage
     */
    void initPage();

    void resizeEvent( QResizeEvent * event );

public slots:
    void onShowBrowser();
    void onShowHelp();
    void onShowComposer();

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
     * @brief Display a licenses window for external libraries
     */
    void onShowExtLicenses();

    /**
     * @brief Display a database window
     */
    void onShowDatabase();

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

signals:
    /**
     * @deprecated Doesn't use the medMainWindow's slots anymore.
     */
    void showSettings();
    /**
     * @deprecated Doesn't use the medMainWindow's slots anymore.
     */
    void showAbout();
    /**
     * @deprecated Doesn't use the medMainWindow's slots anymore.
     */
    void showHelp();

    void showBrowser();
    void showViewer();
    void showWorkspace(QString workspace);
    void showComposer();

protected:
    void expandDetailedText(QMessageBox*);

private:
    medHomepageAreaPrivate * d;
};
