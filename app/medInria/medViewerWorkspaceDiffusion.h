#ifndef MEDVIEWERWORKSPACEDIFFUSION_H
#define MEDVIEWERWORKSPACEDIFFUSION_H


#include <QtCore>

#include <medViewerWorkspace.h>
#include <medToolBoxDiffusionTensorView.h>
#include <medToolBoxDiffusionFiberView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>


class medTabbedViewContainers;
class medViewerWorkspaceDiffusionPrivate;

class medViewerWorkspaceDiffusion : public medViewerWorkspace
{
    Q_OBJECT

public:
     medViewerWorkspaceDiffusion(QWidget *parent = 0);
    ~medViewerWorkspaceDiffusion();

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
    void onTBDiffusionSuccess(void);
    void refreshInteractors (void);

    /**
      * @brief Adds a new tab to a workspace
      *
      * Re-implemented, replaces default implementation in medViewerWorkspace
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

private:
    medViewerWorkspaceDiffusionPrivate *d;

    /** Updates the tensor interactor with the current values in the tensor toolbox. */
    void updateTensorInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medToolBoxDiffusionTensorView* tensorViewToolBox);

    /** Updates the fiber interactor with the current values in the fiber toolbox. */
    void updateFiberInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medToolBoxDiffusionFiberView* fiberViewToolBox);
};


#endif // MEDVIEWERWORKSPACEDIFFUSION_H
