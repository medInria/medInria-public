

#ifndef medViewerConfigurationSequenceRegistration_H
#define medViewerConfigurationSequenceRegistration_H

#include <QtCore>

#include <medGui/medViewerConfiguration.h>


class medViewerConfigurationSequenceRegistrationPrivate;

class medViewerConfigurationSequenceRegistration : public medViewerConfiguration
{
    Q_OBJECT

public:
     medViewerConfigurationSequenceRegistration(QWidget *parent = 0);
    ~medViewerConfigurationSequenceRegistration(void);
    virtual void setupViewContainerStack();
    virtual QString description(void) const;

public slots:


private:
    medViewerConfigurationSequenceRegistrationPrivate *d;
};

medViewerConfiguration *createMedViewerConfigurationSequenceRegistration(void);

#endif
