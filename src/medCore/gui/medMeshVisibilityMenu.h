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

#include <medAbstractParameterGroup.h>


class medMeshVisibilityMenuPrivate;


class MEDCORE_EXPORT medMeshVisibilityMenu : public QPushButton
{
    Q_OBJECT

public:
    medMeshVisibilityMenu(QWidget * parent = 0);
    virtual ~medMeshVisibilityMenu();
    void setAvailableLabels(QStringList parameters);
    void setLabels(QList<double*> * listValues);
    void setAnatomicalLabelsForMesh(QList<QPair<QString,int> > );
    QList<QPair<QString,int> > getAllAnatomicalLabels();
    QList<QPair<QString,int> > getAnatomicalLabelsForMesh();

signals:
    void labelChecked(int);
    void labelUnchecked(int);

private slots:
    void showPopup();
    void selectLabel(QListWidgetItem *);
    //void showSubMenu(QListWidgetItem *);
    //void showSubMenu();
    void hideSubMenu();
    void highlightItem(QListWidgetItem*);
    void selectItem(QListWidgetItem*);

protected:
    bool eventFilter(QObject *object, QEvent *event);
    void paintEvent(QPaintEvent *);

private:
    void updateListsPosition();
    void hideMenus();
    void checkAllLabels(bool check);

private:
    medMeshVisibilityMenuPrivate *d;
};










