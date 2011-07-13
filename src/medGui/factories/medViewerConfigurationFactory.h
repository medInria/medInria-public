#ifndef MEDVIEWERCONFIGURATIONFACTORY_H
#define MEDVIEWERCONFIGURATIONFACTORY_H

#include "medGuiExport.h"

#include <dtkCore/dtkAbstractFactory.h>

#include <QtCore>

class medViewerConfiguration;
class medViewerConfigurationFactoryPrivate;

class MEDGUI_EXPORT medViewerConfigurationFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medViewerConfiguration *(*medViewerConfigurationCreator)(void);

    typedef QHash<QString, medViewerConfigurationCreator> medViewerConfigurationCreatorHash;

public:
    static medViewerConfigurationFactory *instance(void);

    bool registerConfiguration(QString type, medViewerConfigurationCreator func);

    QList<QString> configurations(void);

public slots:
    medViewerConfiguration *createConfiguration(QString type);

protected:
     medViewerConfigurationFactory(void);
    ~medViewerConfigurationFactory(void);

private:
    static medViewerConfigurationFactory *s_instance;

private:
    medViewerConfigurationFactoryPrivate *d;
};

#endif // MEDVIEWERCONFIGURATIONFACTORY_H
