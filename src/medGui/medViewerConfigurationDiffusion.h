#ifndef MEDVIEWERCONFIGURATIONDIFFUSION_H
#define MEDVIEWERCONFIGURATIONDIFFUSION_H


#include <QtCore>

#include "medViewerConfiguration.h"

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

private:
    medViewerConfigurationDiffusionPrivate *d;
};

medViewerConfiguration *createMedViewerConfigurationDiffusion(void);


#endif // MEDVIEWERCONFIGURATIONDIFFUSION_H
