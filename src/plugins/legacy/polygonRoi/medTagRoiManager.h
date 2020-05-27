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
#include <medWorldPosContours.h>
#include <polygonRoi.h>

class medTagRoiManagerPrivate;
class polygonEventFilter;

typedef itk::Image<unsigned char, 3> UChar3ImageType;

class POLYGONROIPLUGIN_EXPORT medTagRoiManager : public QObject
{
    Q_OBJECT

public:
    medTagRoiManager(medAbstractView *view, polygonEventFilter *eventCursor, QColor color=Qt::transparent, QString name="undefined");
    ~medTagRoiManager();

    polygonRoi* roiOpenInSlice();
    bool roiClosedInSlice();
    bool isSameSliceOrientation();
    polygonRoi *existingRoiInSlice();
    void appendRoi(polygonRoi *roi);
    QList<polygonRoi *> getRois();
    QColor getColor();
    QColor getOptColor();
    QString getName();
    QString getOptName();
    polygonRoi *appendRoi();

    void setEnableInterpolation(bool state);

    void manageTick(medSliderL *slider);
    void manageVisibility();
    bool mouseIsCloseFromNodes(double mousePos[2]);
    double getMinimumDistanceFromNodesToMouse(double eventPos[2], bool allNodes = true);
    void deleteNode(double X, double Y);
    void deleteContour();
    void removeAllTick();
    void createMaskWithLabel(int label);
    void SetMasterRoi(bool state);

    vtkSmartPointer<vtkPolyData> getContoursAsPolyData(int label);
    QVector<medWorldPosContours> getContoursAsNodes();
    void loadContours(QVector<medWorldPosContours> contours);

    int getClosestSliceFromCurrent2DView();

    QVector<QVector2D> copyContour();
    bool pasteContour(QVector<QVector2D> nodes);
    void setName(QString name);
    void setColor(QColor color);
    void setOptionalNameWithColor(QString name, QColor color);
    void removeContourOtherView(medAbstractImageView *v);
    void removeIntermediateContoursOtherView(medAbstractImageView *v);
    void activateContours(bool state);
    void changeContoursColor(QColor color);
    bool hasScore();
    void setScoreState(bool state);
    QColor switchColor();

public slots:
    void interpolateIfNeeded();
    void enableOtherViewVisibility(medAbstractImageView *v, bool state);

signals:
    void enableOtherViewsVisibility(bool state);
    void sendErrorMessage(QString);
private:
    dtkSmartPointer<medAbstractData> output;
    dtkSmartPointer<medAbstractData> contourOutput;
    bool isSameOrientation(int orientation);
    medTagRoiManagerPrivate* const d;

    QList<polygonRoi *> interpolateBetween2Slices(polygonRoi *firstRoi, polygonRoi *secondRoi);
    QList<QVector<QVector3D> > generateIntermediateCurves(vtkSmartPointer<vtkPolyData> curve1, vtkSmartPointer<vtkPolyData> curve2, int nb);
    void resampleCurve(vtkPolyData *poly, int nbPoints);
    void reorderPolygon(vtkPolyData *poly);
    static bool sortRois(const polygonRoi *p1, const polygonRoi *p2);
    void connectRois();
    double getDistance(double mousePos[2], double contourPos[2]);
    void initializeMaskData(medAbstractData *imageData, medAbstractData *maskData);
    void removeIntermediateContours();
};


