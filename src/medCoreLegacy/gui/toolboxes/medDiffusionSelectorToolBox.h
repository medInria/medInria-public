/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medToolBox.h"
#include "medCoreExport.h"

class medAbstractDiffusionProcess;
class medAbstractImageData;
class medDiffusionSelectorToolBoxPrivate;
class medDataIndex;

class MEDCORE_EXPORT medDiffusionSelectorToolBox : public medToolBox
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

    void addInputImage(medAbstractImageData *data);
    void clearInputs();

    void setProcessParameters(medAbstractDiffusionProcess *process);

public slots:
    void clear();

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


