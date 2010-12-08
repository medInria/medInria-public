#ifndef MEDVIEWERCONFIGURATIONREGISTRATION_H
#define MEDVIEWERCONFIGURATIONREGISTRATION_H


#include <QtCore>

#include "medViewerConfiguration.h"

class medViewerConfigurationRegistrationPrivate;

class medViewerConfigurationRegistration : public medViewerConfiguration
{
    Q_OBJECT

public:
     medViewerConfigurationRegistration(QWidget *parent = 0);
    ~medViewerConfigurationRegistration(void);

    virtual QString description(void) const;

private:
    medViewerConfigurationRegistrationPrivate *d;
};

medViewerConfiguration *createMedViewerConfigurationRegistration(void);


#endif // MEDVIEWERCONFIGURATIONREGISTRATION_H
