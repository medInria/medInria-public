/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medGuiExport.h>
#include <QtGui>


class medLinkMenuPrivate;

class MEDGUI_EXPORT medLinkMenu : public QPushButton
{
    Q_OBJECT

public:
    medLinkMenu(QWidget * parent = 0);
    void setAvailableParameters(QStringList parameters);

private slots:
    void showPopup();
    void createNewGroup();
    void updateGroupEditOnFocusIn();
    void updateGroupEditOnFocusOut();
    void selectItem(QListWidgetItem *);
    void showSubMenu(QListWidgetItem *);
    void showSubMenu();
    void hideSubMenu();
    void highLightParam(QListWidgetItem*);
    void deleteGroup();

protected:
    bool eventFilter(QObject *object, QEvent *event);
    void paintEvent(QPaintEvent *);

private:
    void updateListsPosition();
    void hideMenus();

private:
    medLinkMenuPrivate *d;
};


class medGroupWidget : public QWidget
{
    Q_OBJECT

public:
    medGroupWidget(QString groupName, QWidget *parent = 0);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

signals:
    void enterEvent();
    void leaveEvent();
    void deletionRequested();

};


class medLeftArrow : public QWidget
{
    Q_OBJECT

public:
    medLeftArrow(QWidget * parent = 0): QWidget(parent){ this->setFixedSize(15,15);}

    void paintEvent(QPaintEvent*);
};

