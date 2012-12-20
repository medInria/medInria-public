#ifndef medDiffusionWorkspace_H
#define medDiffusionWorkspace_H


#include <QtCore>

#include <medViewerWorkspace.h>
#include <medTensorViewToolBox.h>
#include <medFiberViewToolBox.h>
#include <dtkCore/dtkAbstractViewInteractor.h>


class medTabbedViewContainers;
class medDiffusionWorkspacePrivate;

class medDiffusionWorkspace : public medViewerWorkspace
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
    medDiffusionWorkspacePrivate *d;

    /** Updates the tensor interactor with the current values in the tensor toolbox. */
    void updateTensorInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medTensorViewToolBox* tensorViewToolBox);

    /** Updates the fiber interactor with the current values in the fiber toolbox. */
    void updateFiberInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medFiberViewToolBox* fiberViewToolBox);
};


#endif // medDiffusionWorkspace_H
