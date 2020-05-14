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
#include <medContourInfo.h>
#include <medDisplayPosContours.h>
#include <medTagContours.h>
#include <medTagRoiManager.h>
#include <medViewEventFilter.h>
#include <vtkInriaInteractorStylePolygonRepulsor.h>
#include <vtkMetaDataSet.h>


typedef itk::Image<unsigned char, 3> UChar3ImageType;

class medTableWidgetItem;
class View2DObserver;

class polygonEventFilter : public medViewEventFilter
{
    Q_OBJECT
public:
    polygonEventFilter(medAbstractImageView *view, QList<QColor> colorsList);
    ~polygonEventFilter();
    bool mousePressEvent(medAbstractView * view, QMouseEvent *mouseEvent) override;
    bool mouseReleaseEvent(medAbstractView * view, QMouseEvent *mouseEvent) override;
    bool mouseMoveEvent(medAbstractView * view, QMouseEvent *mouseEvent) override;

    bool eventFilter(QObject *obj, QEvent *event) override;

    void reset();
    void updateView(medAbstractImageView *view);
    void Off();
    void On();
    void setEnableInterpolation(bool state);
    void addAlternativeViews(medAbstractImageView *view);
    void activateRepulsor(bool state);
    void saveMask();
    medAbstractImageView *getView(){return currentView;}
    void clearAlternativeViews();
    bool isContourInSlice();
    void saveAllContours();
    void loadContours(medTagContours tagContours,
                      QColor color);

    void clearCopiedContours();
    void removeObserver();
    void addObserver();

    void updateManagerInfos(QList<medContourInfo> infos);
    void updateContourState(QString name, QColor color, bool score=false);
    void changeContourName(QString name, QColor color);
    void deleteLabel(medContourInfo info);
    QList<medTagRoiManager*> getManagers(){return managers;}

public slots:
    void enableOtherViewsVisibility(bool state);
    void setCursorState(CURSORSTATE state){cursorState = state;}

    void manageTick();
    void manageRoisVisibility();
    void removeView();

    void copyContours();
    void pasteContours();

private slots:
    void deleteNode(medTagRoiManager *manager, const double *mousePos);
    void deleteContour(medTagRoiManager *manager);
    void deleteLabel(medTagRoiManager *manager);
    void saveMask(medTagRoiManager *manager);
    void saveContour(medTagRoiManager *manager);
    void copyContour(medTagRoiManager *manager);

signals:
    void enableRepulsor(bool state);
    void enableGenerateMask(bool state);
    void enableViewChooser(bool state);
    void toggleRepulsorButton(bool);
    void clearLastAlternativeView();
    void sendErrorMessage(QString);
    void sendContourInfoToListWidget(medContourInfo &contourInfo);
private:
    medAbstractImageView *currentView;
    dtkSmartPointer<medAbstractData> contourOutput;
    CURSORSTATE cursorState;
    QList<medTagRoiManager *> managers;
    QColor activeColor;
    QString activeName;
    bool scoreState;
    QList<medAbstractImageView*> otherViews;
    bool isRepulsorActivated;
    vtkInriaInteractorStylePolygonRepulsor *interactorStyleRepulsor;
    QList<medDisplayPosContours> copyNodesList;
    bool activateEventFilter;
    bool enableInterpolation;
    medTagRoiManager *activeManager;
    vtkSmartPointer<View2DObserver> observer;
    double savedMousePosition[2];

    bool leftButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent);
    bool rightButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent);
    bool addPointInContourWithLabel(QMouseEvent *mouseEvent);
    void setToolboxButtonsState(bool state);
    void manageButtonsState();
    void saveContoursAsMedAbstractData(vtkMetaDataSet *outputDataSet, QVector<medTagContours> contoursData);
    medTagRoiManager *getManagerFromColor(QColor color);
    QLineEdit *updateNameManager(medTagRoiManager* closestManager, QMenu *mainMenu);
    void deleteNode(double *mousePosition);
    medTagRoiManager *getClosestManager(double *mousePos);
    void enableOnlyActiveManager();
    QMenu *changeLabelActions(medTagRoiManager* closestManager);
    bool isActiveContourInSlice();
    void setCustomCursor();
    QString getManagerName(QString labelName, int index);
    QAction *createScoreAction(medTagRoiManager *manager, QString score, QColor color);
    void switchContourColor(double *mousePosition);
    medTagRoiManager *findManagerWithColor(QColor color);
    void createNewManager(medTagContours tagContours);
    medTagRoiManager *createManager();
};
