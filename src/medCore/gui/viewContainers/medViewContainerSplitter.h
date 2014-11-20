/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QSplitter>

#include <medCoreExport.h>

class medViewContainer;
class medDataIndex;
struct QUuid;

class MEDCORE_EXPORT medViewContainerSplitter: public QSplitter
{
    Q_OBJECT
public:
    medViewContainerSplitter(QWidget* parent = 0);
    virtual ~medViewContainerSplitter();

    QList<medViewContainer*> containers();
    void adjustContainersSize();

public slots:
    void addViewContainer(medViewContainer* container);
    void insertViewContainer(int index, medViewContainer* container);
    medViewContainer* split(Qt::AlignmentFlag alignement = Qt::AlignRight);
    medViewContainer* splitVertically(medViewContainer *sender);
    medViewContainer* splitHorizontally(medViewContainer *sender);
    medViewContainer* split(medViewContainer *sender, Qt::AlignmentFlag alignement = Qt::AlignRight);
    void split(medDataIndex index, Qt::AlignmentFlag alignement = Qt::AlignRight);
    medViewContainer* split(medViewContainer *sender, Qt::AlignmentFlag alignement,medViewContainer *newContainer);

signals:
    void newContainer(QUuid);
    void containerRemoved();

    /**
     * @brief aboutTobedestroyed
     * It is emited right from the start of the destructor. It is used by the medTabbedViewContainer
     * to repopulate the current tab when the last container is removed. We can't use the destroyed
     * signal because it is emited too late.
     * We want it to be emited before the splitter get unparented, otherwise the tab is already remooved.
     */
    void aboutTobedestroyed();

private slots:
    medViewContainer* splitVertically();
    medViewContainer* splitHorizontally();

    void checkIfStillDeserveToLive();

private:
    void recomputeSizes(int requestIndex, int newIndex, int newSize);

    void insertNestedSplitter(int index,
                           medViewContainer *oldContainer,
                           medViewContainer *newContainer,
                           bool inverseOrderInSplitter = false);
};
