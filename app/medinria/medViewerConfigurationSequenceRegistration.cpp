

#include "medViewerConfigurationSequenceRegistration.h"
#include <medGui/medToolBoxiLogRegistration.h>

#include <medGui/viewContainers/medViewContainer.h>
#include <medGui/medStackedViewContainers.h>


class medViewerConfigurationSequenceRegistrationPrivate
{
public:
    medToolBoxiLogRegistration   *logToolBox;

};

medViewerConfigurationSequenceRegistration::medViewerConfigurationSequenceRegistration(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationSequenceRegistrationPrivate)
{
    
    //setLayoutToolBoxVisibility(false);

    d->logToolBox = new medToolBoxiLogRegistration(parent);
    this->addToolBox( d->logToolBox );
    
    this->setLayoutType(medViewerConfiguration::LeftDbRightTb);
}

void medViewerConfigurationSequenceRegistration::setupViewContainerStack()
{

}

medViewerConfigurationSequenceRegistration::~medViewerConfigurationSequenceRegistration(void)
{
    delete d;
    d = NULL;
}


QString medViewerConfigurationSequenceRegistration::description(void) const
{
    return "SequenceRegistration";
}

medViewerConfiguration *createMedViewerConfigurationSequenceRegistration(void)
{
    return new medViewerConfigurationSequenceRegistration;
}
