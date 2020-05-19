/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

// medInria
#include <medToolBox.h>
#include <polygonEventFilter.h>


/*! \brief Toolbox to manage list of contours label
 *
 */
class contoursManagementToolBox : public medToolBox
{
    Q_OBJECT

    MED_TOOLBOX_INTERFACE("Contours Management",
                          "contoursManagementToolBox",
                          <<"contoursManagementToolBox")

public:
    contoursManagementToolBox(QWidget *parent = nullptr);
    ~contoursManagementToolBox();
    static bool registered();
    void clear();

public slots:
     // Override base class
     void updateView();
     void clickActivationButton(bool state);

     void receiveContoursDatasFromView(medContourInfo &info);
     void showCurrentLabels();
     void unselectAll();
private slots:
     void showWidgetListForIndex(int index);

     void addLabel();
     void removeLabelNameInList();

signals:
     void sendDatasToView(QList<medContourInfo> infos);
     void sendContourState(medContourInfo info);
     void sendContourName(medContourInfo info);
     void labelToDelete(medContourInfo info);
     void contoursToLoad(medTagContours tagContours,
                           QColor color);
     void repulsorState();
private:
    QComboBox *specialities;
    QList<QListWidget*> labels;
    QPushButton *plusButton;
    QPushButton *minusButton;

    QList<QColor> colors;
    medAbstractView * currentView;
    QPair<int, int> savedSelectedIndex;
    void disableButtons();

    QListWidget *initLabelsList(QStringList names, bool isProstate=false);
    QListWidgetItem *createWidgetItem(QString name, QColor col);
    void updateLabelNamesOnContours(QListWidget *widget);
    QColor findAvailableColor(QListWidget *widget);
    bool loadDataAsContours(medAbstractImageView *v, unsigned int layer);
    void parseJSONFile();
};
