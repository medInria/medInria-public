/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>
#include <medCoreExport.h>

class medViewEventFilter;
class medSegmentationAbstractToolBox;
class medAbstractProcess;

class medSegmentationSelectorToolBoxPrivate;
class MEDCORE_EXPORT medSegmentationSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
    //TODO what is it for ? - RDE
    struct MaskPixelValues {enum E{ Unset = 0, Foreground = 1, Background = 2 };};

     medSegmentationSelectorToolBox(QWidget *parent = 0);
    ~medSegmentationSelectorToolBox();

     medSegmentationAbstractToolBox* currentToolBox();

signals:
     void installEventFilterRequest(medViewEventFilter *filter);

public slots:
    void changeCurrentToolBox(int index);


private:
    medSegmentationSelectorToolBoxPrivate *d;
};


