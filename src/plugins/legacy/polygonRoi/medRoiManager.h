/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <polygonRoiPluginExport.h>

#include <medAbstractRoi.h>
#include <medAbstractView.h>
#include <polygonRoi.h>

class medRoiManagerPrivate;
class medSeriesOfRoi;
class medLabelManagerPrivate;
class PolygonEventFilter;

class POLYGONROIPLUGIN_EXPORT medLabelManager : public QObject
{
    Q_OBJECT

public:
    medLabelManager(medAbstractView *view, int label, PolygonEventFilter *eventCursor);

    bool roiAlreadyInManager(polygonRoi *roiToCheck, int label, int orientation);
    bool roiInSlice(polygonRoi *roiToCheck);
    polygonRoi* openRoiInSlice(int slice, int orientation);
    bool roiAlreadyInSlice(int slice, int orientation);
    void appendRoi(polygonRoi *roi);
    int getLabel();
    QList<polygonRoi *> getRois();
    void removeEvent();
    QColor getColor();
    void appendRoi();

public slots:
    void interpolateIfNeeded();

private:
    bool isSameLabel(int label);
    bool isSameOrientation(int orientation);
    medLabelManagerPrivate* const d;

    QList<polygonRoi *> interpolateBetween2Slices(polygonRoi *firstRoi, polygonRoi *secondRoi);
    QList<vtkPolyData *> generateIntermediateCurves(vtkSmartPointer<vtkPolyData> curve1, vtkSmartPointer<vtkPolyData> curve2, int nb);
    void resampleCurve(vtkPolyData *poly, int nbPoints);
    void reorderPolygon(vtkPolyData *poly);
    static bool sortRois(const polygonRoi *p1, const polygonRoi *p2);
    void connectRois();
    void manageVisibility();
};


