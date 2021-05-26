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
#include <medContourSharedInfo.h>
#include <medDisplayPosContours.h>
#include <medTagContours.h>
#include <medTagRoiManager.h>
#include <medViewEventFilter.h>
#include <vtkInriaInteractorStylePolygonRepulsor.h>
#include <vtkMetaDataSet.h>

class medTableWidgetItem;
class View2DObserver;

class polygonEventFilter : public medViewEventFilter
{
    Q_OBJECT
public:
    polygonEventFilter(medAbstractImageView *view);
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
    void clearAlternativeView();
    bool isContourInSlice();
    void saveAllContours();

    void clearCopiedContours();
    void removeObserver();
    void addObserver();

    QList<medTagRoiManager*> getManagers(){return managers;}

public slots:
    void enableOtherViewsVisibility(bool state);
    void setCursorState(CURSORSTATE state){cursorState = state;}

    void receiveDatasFromToolbox(QList<medContourSharedInfo> infos);
    void receiveContourState(medContourSharedInfo info);
    void receiveContourName(medContourSharedInfo info);
    void deleteLabel(medContourSharedInfo info);
    void loadContours(medTagContours tagContours,
                      QColor color);

    void manageTick();
    void manageRoisVisibility();
    void removeView();

    void copyContours();
    void pasteContours();

    void receiveActivationState(medContourSharedInfo info);
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
    void sendContourInfoToListWidget(medContourSharedInfo &contourInfo);
    void saveContours(medAbstractImageView *view, vtkMetaDataSet *outputDataSet, QVector<medTagContours> contoursData);

private:
    medAbstractImageView *currentView;
    CURSORSTATE cursorState;
    QList<medTagRoiManager *> managers;
    QColor activeColor;
    QString activeName;
    bool scoreState;
    QList<QPair<QString, QColor>> pirads;
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
    medTagRoiManager *getManagerFromColor(QColor color);
    QLineEdit *changeManagerName(medTagRoiManager* closestManager, QMenu *mainMenu);
    void deleteNode(double *mousePosition);
    medTagRoiManager *getClosestManager(double *mousePos);
    void enableActiveManagerIfExists();
    QMenu *changeLabelActions(medTagRoiManager* closestManager);
    bool isActiveContourInSlice();
    void setCustomCursor();
    QString getManagerName(QString labelName, int index);
    QAction *createScoreAction(medTagRoiManager *manager, QString score, QColor color);
    void switchContourColor(double *mousePosition);
    medTagRoiManager *findManagerWithColor(QColor color);
    void createNewManager(medTagContours tagContours);
    medTagRoiManager *createManager();
    void activateContour(double *mousePosition);
};
