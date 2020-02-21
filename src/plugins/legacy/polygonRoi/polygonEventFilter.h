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

#include <QtGui/qwidgetaction.h>

typedef itk::Image<unsigned char, 3> UChar3ImageType;

class medTableWidgetItem;

class polygonEventFilter : public medViewEventFilter
{
    Q_OBJECT
public:
    polygonEventFilter(medAbstractImageView *view);
    ~polygonEventFilter();
    virtual bool mousePressEvent(medAbstractView * view, QMouseEvent *mouseEvent);
    virtual bool mouseReleaseEvent(medAbstractView * view, QMouseEvent *mouseEvent);

    void removeManagers();
    void updateView(medAbstractImageView *view);
    void Off();
    void On();
    void setEnableInterpolation(bool state);
    void updateAlternativeViews(medAbstractImageView *view, medTableWidgetItem *item);
    void activateRepulsor(bool state);
    void generateMask();
public slots:
    void setCursorState(CURSORSTATE state){cursorState = state;}

    void updateLabel(int label);
    void manageTick();
    void manageRoisVisibility();

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

private:
    medAbstractImageView *currentView;
    CURSORSTATE cursorState;
    QList<medLabelManager *> managers;
    QList<QColor> colorList;
    QSignalMapper signalMapper;
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
};
