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
class QUuid;

class MEDGUI_EXPORT medViewContainerSplitter: public QSplitter
{
    Q_OBJECT
public:
    medViewContainerSplitter(QWidget* parent = 0);
    virtual ~medViewContainerSplitter();

public slots:
    void addViewContainer(medViewContainer* container);
    void insertViewContainer(int index, medViewContainer* container);
    void vSplit();
    void hSplit();
    medViewContainer* split(Qt::AlignmentFlag alignement = Qt::AlignRight);
    void split(medDataIndex index, Qt::AlignmentFlag alignement = Qt::AlignRight);
    void checkIfStillDeserveToLive();

signals:
    void newContainer(QUuid&);

private:
    void recomputeSizes(int requestIndex, int newIndex, int newSize);
    void addNestedSplitter(int index,
                           medViewContainer *oldContainer,
                           medViewContainer *newContainer);
};
