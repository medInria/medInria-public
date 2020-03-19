/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

// qt
#include <QMenu>

// medInria
#include <medTagContours.h>
#include <medTagRoiManager.h>
#include <medViewEventFilter.h>
#include <vtkInriaInteractorStylePolygonRepulsor.h>
#include <vtkMetaDataSet.h>


typedef itk::Image<unsigned char, 3> UChar3ImageType;

class medTableWidgetItem;

class polygonEventFilter : public medViewEventFilter
{
    Q_OBJECT
public:
    polygonEventFilter(medAbstractImageView *view);
    ~polygonEventFilter();
    bool mousePressEvent(medAbstractView * view, QMouseEvent *mouseEvent) override;
    bool mouseReleaseEvent(medAbstractView * view, QMouseEvent *mouseEvent) override;

    void reset();
    void updateView(medAbstractImageView *view);
    void Off();
    void On();
    void setEnableInterpolation(bool state);
    void addAlternativeViews(medAbstractImageView *view);
    void activateRepulsor(bool state);
    void saveMask();
    medAbstractImageView *getView(){return currentView;}
    void clearAlternativeViews();
    bool isContourInSlice();
    //void exportContours(QFile *file);
    void saveAllContours();
    void loadContours(medAbstractData *data);

public slots:
    void addRoisInAlternativeViews();
    void setCursorState(CURSORSTATE state){cursorState = state;}

    void updateLabel(int label);
    void manageTick();
    void manageRoisVisibility();
    void removeView();

private slots:
    void deleteNode(medTagRoiManager *manager, double X, double Y);
    void deleteContour(medTagRoiManager *manager);
    void deleteLabel(medTagRoiManager *manager);
    void saveMask(medTagRoiManager *manager);
    void saveContour(medTagRoiManager *manager);

signals:
    void enableRepulsor(bool state);
    void enableGenerateMask(bool state);
    void enableViewChooser(bool state);
    void toggleRepulsorButton(bool);
    void clearLastAlternativeView();

private:
    medAbstractImageView *currentView;
    dtkSmartPointer<medAbstractData> contourOutput;
    CURSORSTATE cursorState;
    QList<medTagRoiManager *> managers;
    QList<QColor> colorList;
    QList<medAbstractImageView*> alternativeViews;
    bool isRepulsorActivated;
    vtkInriaInteractorStylePolygonRepulsor *interactorStyleRepulsor;

    bool leftButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent);
    bool rightButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent);
    QList<QColor> getAvailableColors(QList<QColor> colorsToExclude);
    QMenu *updateLabelMenu(QList<QColor> colors);
    QList<QColor> updateColors(QList<QColor> colorsToExclude);
    bool manageRoiWithLabel(QMouseEvent *mouseEvent);
    bool addPointInContourWithLabel(QMouseEvent *mouseEvent);
    medTagRoiManager *closestManagerInSlice(double mousePos[]);
    void setToolboxButtonsState(bool state);
    medTagRoiManager *addManagerToList(int label);
    void manageButtonsState();
    void saveContoursAsMedAbstractData(vtkMetaDataSet *outputDataSet, QVector<medTagContours> contoursData);
    int findClosestSliceFromMouseClick(QVector3D worldMouseCoord);
    int performPropPicking(medAbstractImageView *v, QMouseEvent *mouseEvent);
    bool updateMainViewOnChosenSlice(medAbstractImageView *view, QMouseEvent *mouseEvent);
    int findAvailableLabel();
    void removeContoursInAlternativeViews(medTagRoiManager *manager);
};
