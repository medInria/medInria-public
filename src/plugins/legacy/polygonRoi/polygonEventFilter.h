/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include "vtkInriaInteractorStylePolygonRepulsor.h"

#include <medViewEventFilter.h>
#include <medRoiManager.h>
#include <QSpinBox>

#include <qwidgetaction.h>

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
    void generateMask();
    medAbstractImageView *getView(){return currentView;}
    void clearAlternativeViews();
    bool isContourInSlice();

public slots:
    void addRoisInAlternativeViews();
    void setCursorState(CURSORSTATE state){cursorState = state;}

    void updateLabel(int label);
    void manageTick();
    void manageRoisVisibility();
    void removeView();
private slots:
    void deletedNode(medLabelManager *manager, double X, double Y);
    void deletedContour(medLabelManager *manager);
    void deletedLabel(medLabelManager *manager);
    void savedMask(medLabelManager *manager);

signals:
    void nodeToDelete(medLabelManager *manager, double X, double Y);
    void contourToDelete(medLabelManager *manager);
    void labelToDelete(medLabelManager *manager);
    void maskToSave(medLabelManager *manager);
    void enableRepulsor(bool state);
    void enableGenerateMask(bool state);
    void enableViewChooser(bool state);
    void toggleRepulsorButton(bool);
    void clearLastAlternativeView();

private:
    medAbstractImageView *currentView;
    CURSORSTATE cursorState;
    QList<medLabelManager *> managers;
    QList<QColor> colorList;
    QSignalMapper signalMapper;
    QList<medAbstractImageView*> alternativeViews;
    bool isRepulsorActivated;
    vtkInriaInteractorStylePolygonRepulsor *interactorStyleRepulsor;

    bool leftButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent);
    QList<QColor> getAvailableColors(QList<QColor> colorsToExclude);
    QMenu *updateLabelMenu(QList<QColor> colors);
    QList<QColor> updateColors(QList<QColor> colorsToExclude);
    bool manageRoiWithLabel(QMouseEvent *mouseEvent);
    bool addPointInContourWithLabel(QMouseEvent *mouseEvent);
    medLabelManager *closestManagerInSlice(double mousePos[]);
    void setToolboxButtonsState(bool state);
    void addManagerToList(int label);
    void manageButtonsState();
};
