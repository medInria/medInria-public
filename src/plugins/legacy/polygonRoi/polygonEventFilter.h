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
#include <QWidgetAction>
#include <medDisplayPosContours.h>
#include <medTagContours.h>
#include <medTagRoiManager.h>
#include <medViewEventFilter.h>
#include <vtkInriaInteractorStylePolygonRepulsor.h>
#include <vtkMetaDataSet.h>


typedef itk::Image<unsigned char, 3> UChar3ImageType;

class medTableWidgetItem;
class View2DObserver;

class polygonEventFilter : public medViewEventFilter
{
    Q_OBJECT
public:
    polygonEventFilter(medAbstractImageView *view);
    ~polygonEventFilter();
    bool mousePressEvent(medAbstractView * view, QMouseEvent *mouseEvent) override;
    bool mouseReleaseEvent(medAbstractView * view, QMouseEvent *mouseEvent) override;
    bool mouseMoveEvent(medAbstractView * view, QMouseEvent *mouseEvent) override;

    bool eventFilter(QObject *obj, QEvent *event) override;

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
    void saveAllContours();
    void loadContours(medAbstractData *data);

    void clearCopiedContours();
    void removeObserver();
    void addObserver();
public slots:
    void enableOtherViewsVisibility(bool state);
    void setCursorState(CURSORSTATE state){cursorState = state;}

    void createNewManager(int label);
    void manageTick();
    void manageRoisVisibility();
    void removeView();

    void copyContours();
    void pasteContours();
private slots:
    void deleteNode(medTagRoiManager *manager, const double *mousePos);
    void deleteContour(medTagRoiManager *manager);
    void deleteLabel(medTagRoiManager *manager);
    void saveMask(medTagRoiManager *manager);
    void saveContour(medTagRoiManager *manager);

    void copyContour(medTagRoiManager *manager);
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
    QList<medAbstractImageView*> otherViews;
    bool isRepulsorActivated;
    vtkInriaInteractorStylePolygonRepulsor *interactorStyleRepulsor;
    QList<medDisplayPosContours> copyNodesList;
    bool activateEventFilter;
    bool enableInterpolation;
    medTagRoiManager *activeManager;
    vtkSmartPointer<View2DObserver> observer;
    double savedMousePosition[2];

    bool leftButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent);
    bool rightButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent);
    QList<QColor> getAvailableColors(QList<QColor> colorsToExclude);
    QMenu *createColorMenu(QList<QColor> colors);
    QList<QColor> updateColorsList(QList<QColor> colorsToExclude);
    bool manageRoiWithLabel(QMouseEvent *mouseEvent);
    bool addPointInContourWithLabel(QMouseEvent *mouseEvent);
    medTagRoiManager *closestManagerInSlice(double mousePos[]);
    void setToolboxButtonsState(bool state);
    medTagRoiManager *addManagerToList(int label, QString labelName);
    void manageButtonsState();
    void saveContoursAsMedAbstractData(vtkMetaDataSet *outputDataSet, QVector<medTagContours> contoursData);
    int findClosestSliceFromMouseClick(QVector3D worldMouseCoord);
    bool updateMainViewOnChosenSlice(medAbstractImageView *view, QMouseEvent *mouseEvent);
    int findAvailableLabel();
    medTagRoiManager *getManagerFromColor(QColor color);
    QWidgetAction * updateNameManager(medTagRoiManager* closestManager, QMenu *mainMenu);
    void deleteNode(double *mousePosition);
    bool isOnlyOneNodeInSlice();
    medTagRoiManager *getClosestManager(double *mousePos);
};
