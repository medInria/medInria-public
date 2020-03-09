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

#include <itkImage.h>
#include <medAbstractRoi.h>
#include <medAbstractView.h>
#include <polygonRoi.h>

class medRoiManagerPrivate;
class medSeriesOfRoi;
class medLabelManagerPrivate;
class polygonEventFilter;
class medTableWidgetItem;

typedef itk::Image<unsigned char, 3> UChar3ImageType;

class POLYGONROIPLUGIN_EXPORT medLabelManager : public QObject
{
    Q_OBJECT

public:
    medLabelManager(medAbstractView *view, polygonEventFilter *eventCursor, QColor color=Qt::transparent);
    ~medLabelManager();

    polygonRoi* roiOpenInSlice();
    bool roiClosedInSlice();
    bool isSameSliceOrientation();
    polygonRoi *existingRoiInSlice();
    void appendRoi(polygonRoi *roi);
    QList<polygonRoi *> getRois();
    QColor getColor();
    void appendRoi();

    void setContourEnabled(bool state);
    void setEnableInterpolation(bool state);

    void manageTick();
    void manageVisibility();
    bool mouseIsCloseFromContour(double mousePos[2]);
    double getMinimumDistanceFromNodesToEventPosition(double eventPos[2]);
    double getMinimumDistanceFromIntermediateNodesToEventPosition(double eventPos[2]);
    void deleteNode(double X, double Y);
    void deleteContour();
    void removeAllTick();
    void createMaskWithLabel(int label);
    void SetMasterRoi(bool state);

public slots:
    void interpolateIfNeeded();
    void addRoisInAlternativeViews(medAbstractImageView *v);

signals:
    void updateRoisInAlternativeViews();
    void toggleRepulsorButton(bool state);

private:
    dtkSmartPointer<medAbstractData> output;
    bool isSameOrientation(int orientation);
    medLabelManagerPrivate* const d;

    QList<polygonRoi *> interpolateBetween2Slices(polygonRoi *firstRoi, polygonRoi *secondRoi);
    QList<vtkPolyData *> generateIntermediateCurves(vtkSmartPointer<vtkPolyData> curve1, vtkSmartPointer<vtkPolyData> curve2, int nb);
    void resampleCurve(vtkPolyData *poly, int nbPoints);
    void reorderPolygon(vtkPolyData *poly);
    static bool sortRois(const polygonRoi *p1, const polygonRoi *p2);
    void connectRois();
    double getDistance(double mousePos[2], double contourPos[2]);
    void initializeMaskData(medAbstractData *imageData, medAbstractData *maskData);
};


