#ifndef MEDVIEWERCONFIGURATIONDIFFUSION_H
#define MEDVIEWERCONFIGURATIONDIFFUSION_H


#include <QtCore>

#include <medGui/medViewerConfiguration.h>
#include <medGui/medToolBoxDiffusionTensorView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>


class medViewContainerStack;
class medViewerConfigurationDiffusionPrivate;

class medViewerConfigurationDiffusion : public medViewerConfiguration
{
    Q_OBJECT

public:
     medViewerConfigurationDiffusion(QWidget *parent = 0);
    ~medViewerConfigurationDiffusion(void);

    virtual QString description(void) const;
    
    void setupViewContainerStack (medViewContainerStack *container);

public slots:
    void patientChanged(int patientId);
    
    void onViewAdded   (dtkAbstractView *view);
    void onViewRemoved (dtkAbstractView *view);

    void onFiberColorModeChanged(int);
    void onGPUActivated(bool);
    void onLineModeSelected(bool);
    void onRibbonModeSelected(bool);
    void onTubeModeSelected(bool);
    void onShowBundles (bool);
    void onTBDiffusionSuccess(void);
    void refreshInteractors (void);

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

    /** Updates the controls in the tensor toolbox with the current values of the interactor. */
    void updateTensorInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medToolBoxDiffusionTensorView* tensorViewToolBox);
};

medViewerConfiguration *createMedViewerConfigurationDiffusion(void);


#endif // MEDVIEWERCONFIGURATIONDIFFUSION_H
