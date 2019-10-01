/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medAbstractRoi.h>
#include <medAbstractView.h>
#include <medRoiManager.h>
#include <medToolBox.h>

class medRoiManagementToolBoxPrivate;

//! Roi Management toolbox
class medRoiManagementToolBox : public medToolBox
{
    Q_OBJECT

    MED_TOOLBOX_INTERFACE("Roi Management",
                          "medRoiManagementToolBox",
                          <<"")

public:

    typedef QList<medAbstractRoi*> * ListRois;
    typedef QList<medSeriesOfRoi*> * ListOfSeriesOfRois;
    typedef QPair<unsigned int,unsigned int> PairInd;
    unsigned long sliceObserverTag, orientationObserverTag;

    medRoiManagementToolBox(QWidget *parent = nullptr);
    ~medRoiManagementToolBox();
    static bool registered();

    QHash<medAbstractView*,ListOfSeriesOfRois> * getSeriesOfRoi();

    medAbstractView * getCurrentView();
    QList<PairInd> getSelectedRois();
    void sliceChangedCallback();

    void clear();

public slots:
    // Override base class
    void updateView();
    void updateDisplay();
    void clearDisplay();
    void saveCurrentPageIndex(int);
    void selectRois();
    void unselectRois();
    void deleteRoi(PairInd);

private:
    medRoiManagementToolBoxPrivate *d;
};

class medRoiItemWidget : public QWidget
{
    Q_OBJECT
public:
    typedef QPair<unsigned int,unsigned int> PairInd;

    explicit medRoiItemWidget(QString name, PairInd indexes, QWidget *parent = nullptr);

    virtual ~medRoiItemWidget();

    PairInd getIndex();
    QLabel *getRoiInfo();

signals:
    void deleteWidget(PairInd);

protected slots:
    void emitDeleteWidget();

private:
    QLabel *roiInfo;
    PairInd indexes;
};
