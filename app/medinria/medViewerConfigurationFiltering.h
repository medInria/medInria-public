/* medViewerConfigurationFiltering.cpp --- 
 *
 */

#ifndef medViewerConfigurationFiltering_H
#define medViewerConfigurationFiltering_H

#include <QtCore>

#include <medGui/medViewerConfiguration.h>

class medViewerConfigurationFilteringPrivate;
class medViewContainerStack;
class dtkAbstractData;

class MEDGUI_EXPORT medViewerConfigurationFiltering : public medViewerConfiguration
{
    Q_OBJECT

public:
     medViewerConfigurationFiltering(QWidget *parent = 0);
    ~medViewerConfigurationFiltering(void);
    
    virtual QString description(void) const;
    
    void setupViewContainerStack ();

signals:
    void inputDataChanged(dtkAbstractData *);
    void outputDataChanged(dtkAbstractData *);

public slots:    
    void patientChanged(int patientId);
    void onProcessSuccess(void);

private:
    medViewerConfigurationFilteringPrivate *d;
};

MEDGUI_EXPORT medViewerConfiguration *createMedViewerConfigurationFiltering(void);

#endif
