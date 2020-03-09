/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include "medAbstractRoi.h"
#include <medAbstractView.h>
#include <polygonRoiPluginExport.h>
#include <vtkContourWidget.h>
#include <vtkImageView2D.h>
#include <medImageViewEnum.h>
#include <medAbstractImageView.h>
#include <vtkPolygon.h>

enum class CURSORSTATE { CS_NONE, CS_MOUSE_EVENT, CS_SLICE_CHANGED, CS_CONTINUE, CS_REPULSOR  };

class polygonRoiPrivate;
class BezierRoiObserver;
/**
 * 
 */
class POLYGONROIPLUGIN_EXPORT polygonRoi : public medAbstractRoi
{
    Q_OBJECT

public:
    polygonRoi(vtkImageView2D *view, QColor color, medAbstractRoi *parent = nullptr );
    virtual ~polygonRoi();

    vtkContourWidget * getContour();
    void createPolydataToAddInViews();
    vtkImageView2D * getView();
    
    virtual void Off();
    virtual void On();
    virtual bool isVisible();

    virtual QString info();
    virtual QString type();
    virtual void select();
    virtual void unselect();

    virtual bool canRedo(){return true;}
    virtual bool canUndo(){return true;}
    
    void addViewToList(medAbstractImageView *viewToAdd);
    void addRoiToAlternativeView(medAbstractImageView *view);
    bool isClosed();
    void setEnabled(bool state);
    vtkSmartPointer<vtkPolygon> createPolygonFromContour();
    void manageVisibility();

public slots:
    virtual void undo();
    virtual void redo();
    virtual void saveState(){}

signals:
    void updateCursorState(CURSORSTATE state);
    void interpolate();
    void toggleRepulsorButton(bool state);
    void updateRoiInAlternativeViews();

private:
    polygonRoiPrivate *d;
    friend class PolygonRoiObserver;
    void setColor(double[]);
    virtual void setRightColor();
};
