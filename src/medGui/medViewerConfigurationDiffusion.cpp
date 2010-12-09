#include "medViewerConfigurationDiffusion.h"


#include "medViewerToolBoxView.h"
#include "medToolBoxDiffusion.h"
#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerStack.h>

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>

class medViewerConfigurationDiffusionPrivate
{
public:
    medViewerToolBoxView   *viewToolBox;
    medToolBoxDiffusion    *diffusionToolBox;
};

medViewerConfigurationDiffusion::medViewerConfigurationDiffusion(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationDiffusionPrivate)
{
    // -- Layout toolbox --
    setLayoutToolBoxVisibility(false);

    // -- View toolbox --

    d->viewToolBox = new medViewerToolBoxView(parent);

    // -- Registration toolbox --

    d->diffusionToolBox = new medToolBoxDiffusion(parent);

    connect(d->diffusionToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->diffusionToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));

    this->addToolBox( d->viewToolBox );
    this->addToolBox( d->diffusionToolBox );

    this->setViewLayoutType (0); // single
}

medViewerConfigurationDiffusion::~medViewerConfigurationDiffusion(void)
{
    delete d;
    d = NULL;
}

QString medViewerConfigurationDiffusion::description(void) const
{
    return "Diffusion";
}

void medViewerConfigurationDiffusion::setupViewContainerStack(medViewContainerStack *container)
{
    if (!container) {
        return;
    }
    
    medViewContainer *singleContainer = container->single();
    if (singleContainer->current()->view()) {
        d->diffusionToolBox->setView ( singleContainer->current()->view() );
    } 
    else {
        if (dtkAbstractView *view = dtkAbstractViewFactory::instance()->create("v3dView")) {
            singleContainer->current()->setView (view);
            d->diffusionToolBox->setView (view);
        }
    }
}

void medViewerConfigurationDiffusion::patientChanged(int patientId)
{
    d->diffusionToolBox->clear();
}

medViewerConfiguration *createMedViewerConfigurationDiffusion(void)
{
    return new medViewerConfigurationDiffusion;
}
