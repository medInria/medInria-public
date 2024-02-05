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
#include <viewinteractors/polygonRoi.h>

class medLabelPolygonsPrivate;
class baseViewEvent;
class medLabelProperty;

typedef itk::Image<unsigned char, 3> UChar3ImageType;

class POLYGONROIPLUGIN_EXPORT polygonLabel : public QObject
{
    Q_OBJECT


public:
    polygonLabel(medAbstractImageView *view, baseViewEvent *eventCursor, QColor &color, QString &name,
                 int position, bool isSelected, bool interpolate);
    ~polygonLabel() override;

    polygonRoi* roiOpenInSlice();

    [[maybe_unused]] bool roiClosedInSlice();
    bool isSameSliceOrientation();
    bool isRoiInSlice();
    polygonRoi *getRoiInSlice();
    QList<polygonRoi *> getRois();
    QColor & getColor();
    QColor & getOptColor();
    QString & getName();
    QString & getOptName();
    int getPosition();

    polygonRoi *appendRoi();

    void removeViewObservers();
    void addViewObservers();

    void setEnableInterpolation(bool state);

    void manageTick(medSliderL *slider);
    void manageVisibility();
    double getMinimumDistanceFromNodesToMouse(double eventPos[2], bool allNodes = true);
    void deleteNode(double X, double Y);
    void deleteContour();
    void removeAllTick();
    medAbstractData *createMask(int label, QString &desc);
    void SetMasterRoi();

    vtkSmartPointer<vtkPolyData> getContoursAsPolyData(int label);
    QVector<medWorldPosContours> getContoursAsNodes();
    void loadContours(QVector<medWorldPosContours> &contours);

    QVector<QVector2D> copyContour();
    bool pasteContour(QVector<QVector2D> nodes);
    void setName(QString &name);
    void setColor(QColor &color);
    void setOptionalNameWithColor(const QString &name, const QColor &color);
    void setRoisSelectedState();
    bool hasScore();
    void setScoreState(bool state);
    QColor switchColor();
    void updateRoiOnOrientedView(medAbstractImageView *pView, bool state);
    medLabelProperty & getState();
    void updateContourProperty(QString &name, QColor &color, bool isSelected, bool hasScore, bool checkState);
    void changeContoursColor(QColor color);

public slots:
    void interpolateIfNeeded();

signals:
    void enableOtherViewsVisibility(bool state);
    void sendErrorMessage(QString);
private:
    dtkSmartPointer<medAbstractData> output;
    bool isSameOrientation(int orientation);
    medLabelPolygonsPrivate* const d;

    QList<polygonRoi *> interpolateBetween2Slices(polygonRoi *firstRoi, polygonRoi *secondRoi);
    QList<QVector<QVector3D> > generateIntermediateCurves( vtkContourRepresentation* firstContour,  vtkContourRepresentation* secondContour, int nb);
    static void resampleCurve(vtkPolyData *poly, int nbPoints, vtkPolyData *poly2);
    void reorderPolygon(vtkPolyData *poly);
    static bool sortRois(const polygonRoi *p1, const polygonRoi *p2);
    void connectRois();
    static double getDistance(double mousePos[2], double contourPos[2]);
    static void initializeMaskData(medAbstractData *imageData, medAbstractData *maskData);
    void removeIntermediateContours();

    vtkImageView2D * getView2D() const;
};


