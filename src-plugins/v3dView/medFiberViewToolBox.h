/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>
#include "v3dViewPluginExport.h"

class medFiberViewToolBoxPrivate;

class V3DVIEWPLUGIN_EXPORT medFiberViewToolBox : public medToolBox
{
    Q_OBJECT
public:
    medFiberViewToolBox(QWidget *parent);
    ~medFiberViewToolBox();

signals:
    void fiberRadiusSet(int);
    void fiberColorModeChanged(int);
    void GPUActivated(bool);
    void lineModeSelected(bool);
    void ribbonModeSelected(bool);
    void tubeModeSelected(bool);

public:

    /* Returns the selected fiber radius. */
    int fiberRadius();

    /* Returns the selected fiber color mode. */
    int fiberColorMode();

    /* Returns whether the GPU acceleration is activated. */
    bool isGPUActivated();

    /* Returns whether line mode is selected or not. */
    bool isLineModeSelected();

    /* Returns whether ribbon mode is selected or not. */
    bool isRibbonModeSelected();

    /* Returns whether tubes mode is selected or not. */
    bool isTubesModeSelected();

    static bool registered();
public slots:
    void setFiberColorMode(int);
    void activateGPU(bool);
    void selectLineMode(bool);
    void selectRibbonMode(bool);
    void selectTubeMode(bool);
    void setFiberRadius(int radius);

    void update (dtkAbstractView *view);

protected:
    medFiberViewToolBoxPrivate *d;
};


