

#include "medViewerConfigurationSequenceRegistration.h"


#include <medGui/medViewContainer.h>
#include <medGui/medStackedViewContainers.h>


class medViewerConfigurationSequenceRegistrationPrivate
{
public:
    

};

medViewerConfigurationSequenceRegistration::medViewerConfigurationSequenceRegistration(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationSequenceRegistrationPrivate)
{
    
    showLayoutToolBox();

   
    
    
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
