/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QSplitter>

#include <medGuiExport.h>

class medViewContainer;
class medDataIndex;
struct QUuid;

class MEDGUI_EXPORT medViewContainerSplitter: public QSplitter
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
    void checkIfStillDeserveToLive();

signals:
    void newContainer(QUuid);
    void containerRemoved();

private slots:
    medViewContainer* splitVertically();
    medViewContainer* splitHorizontally();

private:
    void recomputeSizes(int requestIndex, int newIndex, int newSize);
    void insertNestedSplitter(int index,
                           medViewContainer *oldContainer,
                           medViewContainer *newContainer,
                           bool inverseOrderInSplitter = false);
};
