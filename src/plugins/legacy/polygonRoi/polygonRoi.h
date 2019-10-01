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

class polygonRoiPrivate;
class BezierRoiObserver;
/**
 * 
 */
class POLYGONROIPLUGIN_EXPORT polygonRoi : public medAbstractRoi
{
    Q_OBJECT

public:
    polygonRoi(vtkImageView2D *view, medAbstractRoi *parent = nullptr );
    virtual ~polygonRoi();

    vtkContourWidget * getContour();
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
    
public slots:
    
    void showOrHide();
    virtual void undo();
    virtual void redo();
    virtual void saveState();
    virtual bool copyROI(medAbstractView *view);
    virtual medAbstractRoi * getCopy(medAbstractView *view);
    virtual QList<medAbstractRoi*> * interpolate(medAbstractRoi *roi);
    
private:
    polygonRoiPrivate *d;
    friend class PolygonRoiObserver;
    void setColor(double[]);
    virtual void setRightColor();
};
