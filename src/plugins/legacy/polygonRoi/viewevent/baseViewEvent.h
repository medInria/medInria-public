/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

// qt
#include <QMenu>

// medInria
#include <QLineEdit>
#include <QListWidget>
#include <QWidgetAction>
#include <medLabelProperty.h>
#include <medDisplayPosContours.h>
#include <medTagContours.h>
#include <polygonLabel.h>
#include <medViewEventFilter.h>
#include <vtkInriaInteractorStylePolygonRepulsor.h>
#include <vtkMetaDataSet.h>
#include <defaultLabelToolBox.h>
#include <medIntParameterL.h>
#include <medAbstractImageData.h>
#include <polygonRoiToolBox.h>
#include <viewinteractors/medMouseCrossPosition.h>

class medTableWidgetItem;
class defaultLabelToolBox;

class baseViewEvent : public medViewEventFilter
{
    Q_OBJECT

    void activateMouseEvent(bool state);

public:
    baseViewEvent(medAbstractImageView *iView, polygonRoiToolBox *toolBox);
    ~baseViewEvent();
    bool mousePressEvent(medAbstractView * view, QMouseEvent *mouseEvent) override;
    bool mouseReleaseEvent(medAbstractView * view, QMouseEvent *mouseEvent) override;
    bool mouseMoveEvent(medAbstractView * view, QMouseEvent *mouseEvent) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

    medAbstractView *getCurrentView();

    virtual void initialize(medToolBox *medTb, QString &seriesName) = 0;

    virtual void updateLabelToolBoxState(QString &name) = 0;
    void updatePosition(QString name, int position);
    void setEnableInterpolation(bool state);
    void activateRepulsor(bool state);
    QList<dtkSmartPointer<medAbstractData> > saveMasks();
    void saveAllContours();

    void removeViewFromList(medAbstractImageView *iView);
    void rename(int position, QString newName);
    void deleteLabel(int position);

    virtual void deleteLabel(polygonLabel *manager) = 0;
    virtual void loadContours(QVector<medTagContours> &tagContoursSet) = 0;
    void drawCross(double *position);
    void eraseCross();
    
public slots:

    void onContourFinished(CURSORSTATE state);

    void updateContourProperty(QString &name, QColor &color, int position, bool isSelected, bool hasScore,
                               bool checkState, QString scoreName = QString());

    void manageTickVisibility(bool show);

    void copyContours();
    void pasteContours();

    void onSelectContainer();
    void removeViewInteractor();
    void showOnDifferentOrientation();

protected slots:
    int deleteLabelBase(polygonLabel *manager);

private slots:
    void deleteNode(polygonLabel *manager, const double *mousePos);
    void deleteContour(polygonLabel *manager);
    dtkSmartPointer<medAbstractData> saveMask(polygonLabel *manager);
    void saveContour(polygonLabel *label);
    void copyContour(polygonLabel *manager);

protected:
    QList<polygonLabel *> labelList;
    polygonLabel *currentLabel;
    medAbstractImageView *currentView;
    QString identifier;
    bool enableInterpolation;
    polygonRoiToolBox *pToolBox;
    medMouseCrossPosition *crossPosition;
    int globalVtkLeftButtonBehaviour;

    int activateContourBase(double *mousePosition);
    static QLineEdit *changeManagerNameBase(polygonLabel *closestManager, QMenu *mainMenu);
    void unselectLabels();
    polygonLabel *getClosestPLabel(double *mousePos);
    bool isContourLoadable(QString &labelName);
    polygonLabel *findManager(int position);
    virtual QString createMaskDescription(polygonLabel *label);

private:
    CURSORSTATE cursorState;
    bool isRepulsorActivated;
    vtkInriaInteractorStylePolygonRepulsor *interactorStyleRepulsor;
    QList<medDisplayPosContours> copyNodesList;
    double savedMousePosition[2];
    dtkSmartPointer<medAbstractData> contourOutput;
    vtkRenderWindowInteractor* originalInteractor;

    void leftButtonBehaviour(medAbstractView *view);
    bool rightButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent);
    void addPointInContourWithLabel();
    void deleteNode(double *mousePosition);
    void setLabelActivationState();
    QMenu *changeLabelActions(polygonLabel* closestLabel);
    bool isActiveContourInSlice();
    static void setCustomCursor(medAbstractView *view, QColor color);

    virtual void activateContour(double *mousePosition) = 0;

    virtual QLineEdit *changeManagerName(polygonLabel* closestManager, QMenu *mainMenu) = 0;

    virtual void switchContourColor(double *mousePosition) = 0;

    virtual QMenu *createScoreMenu(polygonLabel *pLabel) = 0;

    virtual int speciality() = 0;

    virtual void setScoreNameAndColor(polygonLabel *label, QString &name) {};

    static medIntParameterL *getSlicingParameter(medAbstractImageView *iView) ;

};
