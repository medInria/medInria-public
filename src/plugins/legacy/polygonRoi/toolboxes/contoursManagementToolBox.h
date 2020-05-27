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
#include <SwitchButton.h>
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
    static const char* urologySpeciality;

    contoursManagementToolBox(QWidget *parent = nullptr);
    ~contoursManagementToolBox();
    static bool registered();
    void clear();

public slots:
     // Override base class
     void updateView();
     void clickActivationButton(bool state);

     void receiveContoursDatasFromView(medContourSharedInfo &info);
     void showCurrentLabels();
     void unselectAll();
     void onContoursSaved(medAbstractImageView *view,
                          vtkMetaDataSet *outputDataSet,
                          QVector<medTagContours> contoursData);

private slots:
     void showWidgetListForIndex(int index);

     void addLabel();
     void removeLabelNameInList();

     void switchTargetState(bool state);
signals:
     void sendDatasToView(QList<medContourSharedInfo> infos);
     void sendContourState(medContourSharedInfo info);
     void sendActivationState(medContourSharedInfo info);
     void sendContourName(medContourSharedInfo info);
     void labelToDelete(medContourSharedInfo info);
     void contoursToLoad(medTagContours tagContours,
                           QColor color);
     void repulsorState();
private:
    QComboBox *specialities;
    QList<QListWidget*> labels;
    QPushButton *plusButton;
    QPushButton *minusButton;
    SwitchButton* targetButton;
    QLabel *targetLabel;

    dtkSmartPointer<medAbstractData> contourOutput;

    QList<QColor> mainColors;
    QList<QColor> targetColors;
    medAbstractView * currentView;
    QPair<int, int> savedSelectedIndex;
    void disableButtons();

    QListWidget *initLabelsList(QStringList names, QList<bool> scores, bool isProstate=false);
    QListWidgetItem *createWidgetItem(QString name, QColor col, bool score=false, bool isProstate=false);
    void updateLabelNamesOnContours(QListWidget *widget);
    QColor findAvailableColor(QListWidget *widget, QList<QColor> colors);
    bool loadDataAsContours(medAbstractImageView *v, unsigned int layer);
    void parseJSONFile();
    bool loadDefaultContours(QListWidget *widget, QVector<medTagContours> &tagContoursSet);
    bool loadUrologyContours(QListWidget *widget, QVector<medTagContours> &tagContoursSet);
};
