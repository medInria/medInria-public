/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medAbstractImageView.h>
#include <medAbstractRoi.h>
#include <medAbstractView.h>
#include <medWorldPosContours.h>
#include <medImageViewEnum.h>
#include <medSliderL.h>
#include <polygonRoiPluginExport.h>
#include <vtkContourWidget.h>
#include <vtkImageView2D.h>
#include <vtkPolygon.h>

enum class CURSORSTATE { CS_NONE, CS_DEFAULT, CS_REPULSOR  };

class polygonRoiPrivate;
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
    vtkImageView2D * getView();

    void removeObservers();
    void addObservers();

    virtual void Off();
    virtual void On();
    virtual bool isVisible();

    virtual QString info();
    virtual QString type();
    virtual void select();
    virtual void unselect();

    virtual bool canRedo(){return true;}
    virtual bool canUndo(){return true;}
    
    void updateContourOtherView(medAbstractImageView *view, bool state);
    bool isClosed();
    vtkPolyData *createPolyDataFromContour();
    void manageVisibility();

    vtkProperty *createPropertyForPolyData();
    vtkPolyData *getPolyData();
    void loadNodes(QVector<QVector3D> coordinates);

    medWorldPosContours getContourAsNodes();
    void manageTick(medSliderL *slider);

    QVector<QVector2D> copyContour();
    bool pasteContour(QVector<QVector2D> &nodes);
    int getNumberOfNodes();
    void activateContour(bool state);
    void updateColor(QColor &color, bool activate);
    QColor getColor();
    bool isInCurrentSlice();
    void setCurrentSlice();

public slots:
    virtual void undo(){};
    virtual void redo(){};
    virtual void saveState(){}

signals:
    void contourFinished(CURSORSTATE state);

private:
    polygonRoiPrivate *d;
    friend class PolygonRoiObserver;
    virtual void setRightColor();

//    void orientationChanged(vtkImageView2D *view2D);

};
