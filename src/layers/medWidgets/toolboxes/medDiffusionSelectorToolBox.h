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
#include <medWidgetsExport.h>
#include <medAbstractJob.h>

class medAbstractDiffusionModelEstimationProcess;
class medAbstractDiffusionScalarMapsProcess;
class medAbstractTractographyProcess;

class medAbstractImageData;
class medDiffusionSelectorToolBoxPrivate;
class medDataIndex;

class MEDWIDGETS_EXPORT medDiffusionSelectorToolBox : public medToolBox
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

    medAbstractData *processOutput();

public slots:
    void clear();
    void setInputGradientFile();
    void chooseProcess(int id);
    void updateCurrentProcessInput(int index);

signals:
    void jobFinished(medAbstractJob::medJobExitStatus);
    void jobRunning(bool);

protected:
    void checkInputGradientDirections();

private:
    medDiffusionSelectorToolBoxPrivate *d;
};


