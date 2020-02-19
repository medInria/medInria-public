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

class medTableWidgetItem;

class polygonEventFilter : public medViewEventFilter
{
    Q_OBJECT
public:
    polygonEventFilter(medAbstractImageView *view);

    virtual bool mousePressEvent(medAbstractView * view, QMouseEvent *mouseEvent);

    void Off();
    void On();
    void setEnableInterpolation(bool state);
    void updateAlternativeViews(medAbstractImageView *view, medTableWidgetItem *item);
    void activateRepulsor(bool state);

public slots:
    void setCursorState(CURSORSTATE state){cursorState = state;}

    void updateLabel(int label);
    void manageTick();
    void manageRoisVisibility();

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
    void chooseLabelMenu(QList<QColor> colors, QMouseEvent *mouseEvent);
    QList<QColor> updateColors(QList<QColor> colorsToExclude);
    bool manageRoiWithLabel(QMouseEvent *mouseEvent);
    bool addPointInContourWithLabel(QMouseEvent *mouseEvent);
    medLabelManager *closestManagerInSlice(double mousePos[]);
};
