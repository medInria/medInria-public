/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once


#include <QtCore>

#include <medWorkspace.h>
#include <medTensorViewToolBox.h>
#include <medSHViewToolBox.h>
#include <medFiberViewToolBox.h>
#include <dtkCore/dtkAbstractViewInteractor.h>


class medTabbedViewContainers;
class medDiffusionWorkspacePrivate;

class medDiffusionWorkspace : public medWorkspace
{
    Q_OBJECT

public:
     medDiffusionWorkspace(QWidget *parent = 0);
    ~medDiffusionWorkspace();

    virtual QString identifier()  const;
    virtual QString description() const;
    static bool isUsable();
    void setupViewContainerStack();

public slots:

    void onViewAdded   (dtkAbstractView *view);
    void onViewRemoved (dtkAbstractView *view);

    void onFiberColorModeChanged(int);
    void onGPUActivated(bool);
    void onLineModeSelected(bool);
    void onRibbonModeSelected(bool);
    void onTubeModeSelected(bool);
    void onTBDiffusionSuccess();
    void refreshInteractors();

    /**
      * @brief Adds a new tab to a workspace
      *
      * Re-implemented, replaces default implementation in medWorkspace
      */
    void onAddTabClicked();


    // slots for tensor interactions

    /** Event called when a new glyph shape is selected in the tensor toolbox */
    void onGlyphShapeChanged(const QString& glyphShape);

    /** Event called when user wants to flip X axis */
    void onFlipXChanged(bool flipX);

    /** Event called when user wants to flip Y axis */
    void onFlipYChanged(bool flipY);

    /** Event called when user wants to flip Z axis */
    void onFlipZChanged(bool flipZ);

    // slots for SH interactions

    /** Event called when a new polyhedron type to be tesselated is selected in the sh toolbox */
    void onTesselationTypeChanged(const QString& tesselationType);

    /** Event called when a new spherical harmonics basis is selected in the sh toolbox */
    void onTesselationBasisChanged(const QString& tesselationBais);

    /** Event called when user wants to flip X axis */
    void onSHFlipXChanged(bool flipX);

    /** Event called when user wants to flip Y axis */
    void onSHFlipYChanged(bool flipY);

    /** Event called when user wants to flip Z axis */
    void onSHFlipZChanged(bool flipZ);

    /** Event called when user wants to switch in between Spherical Harmonic Basis  */
    void onSHMaxThesisFuncChanged(bool MaxThesis);

private:
    medDiffusionWorkspacePrivate *d;

    /** Updates the tensor interactor with the current values in the tensor toolbox. */
    void updateTensorInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medTensorViewToolBox* tensorViewToolBox);

    /** Updates the controls in the SH toolbox with the current values of the interactor. */
    void updateSHInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medSHViewToolBox* SHViewToolBox);
    
    /** Updates the fiber interactor with the current values in the fiber toolbox. */
    void updateFiberInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medFiberViewToolBox* fiberViewToolBox);
};



