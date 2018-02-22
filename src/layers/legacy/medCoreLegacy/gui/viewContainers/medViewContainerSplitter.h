#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <QSplitter>

#include <medCoreLegacyExport.h>

class medViewContainer;
class medDataIndex;
class QUuid;

class MEDCORELEGACY_EXPORT medViewContainerSplitter: public QSplitter
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

    medViewContainer* splitVertically(medViewContainer *sender);
    medViewContainer* splitHorizontally(medViewContainer *sender);
    medViewContainer* split(Qt::AlignmentFlag alignement = Qt::AlignRight);
    medViewContainer* split(medViewContainer *sender, Qt::AlignmentFlag alignement = Qt::AlignRight);
    void split(medDataIndex index, Qt::AlignmentFlag alignement = Qt::AlignRight);

signals:
    void newContainer(QUuid);

private slots:
    medViewContainer* splitVertically();
    medViewContainer* splitHorizontally();
    void checkIfStillDeserveToLiveSpliter();

private:
    void recomputeSizes(int requestIndex, int newIndex, int newSize);
    void insertNestedSplitter(int index, medViewContainer *oldContainer, medViewContainer *newContainer, bool inverseOrderInSplitter = false);
};
