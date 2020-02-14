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

enum class CURSORSTATE { NONE, FIRST_CLICK, ROI_CLOSED, SLICE_CHANGED, CONTINUE  };

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
    QPair<vtkPolyData *, vtkProperty *> getPoly();
    vtkImageView2D * getView();

    void lockObserver(bool val);
    
    virtual void Off();
    virtual void On();
    virtual bool isVisible();
    virtual void forceInvisibilityOn();
    virtual void forceInvisibilityOff();
    virtual void forceVisibilityOn();
    virtual void forceVisibilityOff();

    virtual QString info();
    virtual QString type();
    virtual void select();
    virtual void unselect();
    virtual void computeRoiStatistics();

    virtual bool canRedo(){return true;}
    virtual bool canUndo(){return true;}
    
    void addViewToList(medAbstractImageView *viewToAdd, medImageView::Orientation orientation);
    void addDataSet();
    void activate();
public slots:
    
    void showOrHide();
    virtual void undo();
    virtual void redo();
    virtual void saveState();
    virtual bool copyROI(medAbstractView *view);
    virtual medAbstractRoi * getCopy(medAbstractView *view);
    virtual QList<medAbstractRoi*> * interpolate(medAbstractRoi *roi);

signals:
    void updateCursorState(CURSORSTATE state);
    void interpolate();
private:
    polygonRoiPrivate *d;
    friend class PolygonRoiObserver;
    void setColor(double[]);
    virtual void setRightColor();
};
