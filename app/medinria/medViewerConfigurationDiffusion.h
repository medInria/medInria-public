#ifndef MEDVIEWERCONFIGURATIONDIFFUSION_H
#define MEDVIEWERCONFIGURATIONDIFFUSION_H


#include <QtCore>

#include <medViewerConfiguration.h>
#include <medToolBoxDiffusionTensorView.h>
#include <medToolBoxDiffusionFiberView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>


class medTabbedViewContainers;
class medViewerConfigurationDiffusionPrivate;

class medViewerConfigurationDiffusion : public medViewerConfiguration
{
    Q_OBJECT

public:
     medViewerConfigurationDiffusion(QWidget *parent = 0);
    ~medViewerConfigurationDiffusion();

    virtual QString identifier()  const;
    virtual QString description() const;

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
      * @brief Adds a new tab to a configuration
      *
      * Re-implemented, replaces default implementation in medViewerConfiguration
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
    medViewerConfigurationDiffusionPrivate *d;

    /** Updates the tensor interactor with the current values in the tensor toolbox. */
    void updateTensorInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medToolBoxDiffusionTensorView* tensorViewToolBox);

    /** Updates the fiber interactor with the current values in the fiber toolbox. */
    void updateFiberInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medToolBoxDiffusionFiberView* fiberViewToolBox);
};

medViewerConfiguration *createMedViewerConfigurationDiffusion(QWidget*);


#endif // MEDVIEWERCONFIGURATIONDIFFUSION_H
