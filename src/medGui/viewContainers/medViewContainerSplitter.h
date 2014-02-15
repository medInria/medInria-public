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

class medViewContainer2;

class medViewContainerSplitterPrivate;
class MEDGUI_EXPORT medViewContainerSplitter: public QSplitter
{
    Q_OBJECT
public:
    medViewContainerSplitter(QWidget* parent = 0);
    ~medViewContainerSplitter();

public slots:
    void addViewContainer(medViewContainer2* container);
    void insertViewContainer(int index, medViewContainer2* container);
    void vSplit();
    void hSplit();
    void checkIfStillDeserveToLive();

private:
    medViewContainerSplitterPrivate *d;
};
