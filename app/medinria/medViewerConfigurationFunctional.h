#ifndef MEDVIEWERCONFIGURATIONFUNCTIONAL_H
#define MEDVIEWERCONFIGURATIONFUNCTIONAL_H

#include <QtCore>

#include <medGui/medViewerConfiguration.h>


class medStackedViewContainers;
class medViewerConfigurationFunctionalPrivate;

class medViewerConfigurationFunctional : public medViewerConfiguration
{
    Q_OBJECT

public:
     medViewerConfigurationFunctional(QWidget *parent = 0);
    ~medViewerConfigurationFunctional(void);

    virtual QString description(void) const;
    
    void setupViewContainerStack();

public slots:
    
    void onViewAdded   (dtkAbstractView *view);
    void onViewRemoved (dtkAbstractView *view);

private:
    medViewerConfigurationFunctionalPrivate *d;

};

medViewerConfiguration *createMedViewerConfigurationFunctional(void);

#endif // MEDVIEWERCONFIGURATIONFUNCTIONAL_H
