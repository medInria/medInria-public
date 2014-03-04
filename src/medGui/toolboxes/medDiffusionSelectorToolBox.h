/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include "medToolBox.h"
#include "medGuiExport.h"

class dtkAbstractProcess;
class dtkAbstractView;
class medAbstractDataImage;
class medDiffusionSelectorToolBoxPrivate;
class medDataIndex;

class MEDGUI_EXPORT medDiffusionSelectorToolBox : public medToolBox
{
    Q_OBJECT
public:
    enum SelectorType
    {
        Estimation = 0,
        ScalarMaps,
        Tractography
    };

     medDiffusionSelectorToolBox(QWidget *parent = 0, SelectorType type = Estimation);
    ~medDiffusionSelectorToolBox();

    void setInputImage(medAbstractDataImage *data);
    void clearInput();

    void setProcessParameters(dtkAbstractProcess *process);

public slots:
    void clear();

    void selectInputImage(const medDataIndex& index);
    void setInputGradientFile();
    void createProcess();

    void chooseToolBox(int id);
    void resetButtons();

signals:
    void processRequested(QString, QString);
    void processCancelled();

protected:
    void checkInputGradientDirections();

private:
    medDiffusionSelectorToolBoxPrivate *d;
};


