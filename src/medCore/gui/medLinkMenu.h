/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <QtGui>
#include <QtWidgets>

#include <medAbstractParameterGroup.h>


class medLinkMenuPrivate;


class MEDCORE_EXPORT medLinkMenu : public QPushButton
{
    Q_OBJECT

public:
    medLinkMenu(QWidget * parent = 0);
    virtual ~medLinkMenu();
    void setAvailableParameters(QStringList parameters);
    void addGroup(medAbstractParameterGroup * group, bool selected = false);
    void setGroups(QList<medAbstractParameterGroup *> groups);
    void setSelectedGroups(QStringList groups);
    void setPartiallySelectedGroups(QStringList groups);

signals:
    void parameterChecked(QString paramName, QString groupName, bool groupChecked);
    void parameterUnchecked(QString paramName, QString groupName, bool groupChecked);

    void groupChecked(QString groupName);
    void groupUnchecked(QString groupName);

    void groupCreated(QString groupName);
    void groupDeleted(QString groupName);

    void groupColorChangeRequest(QString groupName, QColor color);


private slots:
    void showPopup();
    void createNewGroup();
    void updateGroupEditOnFocusIn();
    void updateGroupEditOnFocusOut();
    void selectGroup(QListWidgetItem *);
    void selectParam(QListWidgetItem *);
    void showSubMenu(QListWidgetItem *);
    void showSubMenu();
    void hideSubMenu();
    void highlightItem(QListWidgetItem*);
    void deleteGroup();
    void emitGroupColorChangeRequest(QColor);
    void saveAsPreset();
    void loadPreset();
    void applyPreset(QListWidgetItem*);
    void uncheckAllPresets();
    void highlightPreset();
    void selectItem(QListWidgetItem*);
    void deletePreset();
    void editPreset(QListWidgetItem*);

protected:
    bool eventFilter(QObject *object, QEvent *event);
    void paintEvent(QPaintEvent *);

private:
    void updateListsPosition();
    void hideMenus();
    void checkAllParams(bool check);
    void updateParamCheckState(QString group);
    void addPresetItem(QString preset);
    void addRemovePresetButton(QListWidgetItem* item);

private:
    medLinkMenuPrivate *d;
};


class medListItemWidget : public QWidget
{
    Q_OBJECT

public:
    medListItemWidget(QWidget *parent = 0);

public:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

signals:
    void enterEvent();
    void leaveEvent();
};


class medGroupWidget : public medListItemWidget
{
    Q_OBJECT

public:
    medGroupWidget(QString groupName, QWidget *parent = 0);
    void setColor(QColor color);

signals:
    void deletionRequested();
    void colorChanged(QColor);

private slots:
    void setGroupColor();

private:
    QPushButton *m_labelColorWidget;
    QColor m_color;

};


class medLeftArrow : public QWidget
{
    Q_OBJECT

public:
    medLeftArrow(QWidget * parent = 0): QWidget(parent){ this->setFixedSize(15,15);}

    void paintEvent(QPaintEvent*);
};




