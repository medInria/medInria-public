#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCoreExport.h>
#include <medHomepageButton.h>

#include <QtGui>

class medQuickAccessMenu : public QWidget
{
  Q_OBJECT

public:
    medQuickAccessMenu ( bool vertical, QWidget* parent = nullptr, Qt::WindowFlags f = 0 );

    void reset(bool optimizeLayout);

    void updateCurrentlySelectedRight();
    void updateCurrentlySelectedLeft();
    void updateSelected (QString workspace);

    void switchToCurrentlySelected ();

    void manuallyClickOnWorkspaceButton(QString workspaceName);

protected:
    void focusOutEvent(QFocusEvent * event);

    void mouseMoveEvent (QMouseEvent * event);

    void keyPressEvent ( QKeyEvent * event );
    void keyReleaseEvent ( QKeyEvent * event );

    void createHorizontalQuickAccessMenu();
    void createVerticalQuickAccessMenu();

    void mouseSelectWidget(unsigned int identifier);

signals:
    void menuHidden();
    void homepageSelected();
    void browserSelected();
    void composerSelected();
    void areaSelected(QString);
    void workspaceSelected(QString);
    void searchSelected();

private:
    int currentSelected;

    QList <medHomepagePushButton *> buttonsList;

    //! Frame for alt-tab like shortcut access menu
    QFrame *backgroundFrame;
};
