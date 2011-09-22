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
    typedef medViewerConfiguration *(*medViewerConfigurationCreator)(QWidget* parent);

    typedef QHash<QString, medViewerConfigurationCreator> medViewerConfigurationCreatorHash;

public:
    static medViewerConfigurationFactory *instance(void);

    bool registerConfiguration(QString type, medViewerConfigurationCreator func);

    QList<QString> configurations(void);

public slots:
    /**
     * @brief allocates the memory for a medViewerConfiguration.
     * @param type identifier for the Configuration type.
     * @param parent the parentWidget for all the Widget created in the configuration, even if the configuration is not a widget, its children can be destroyed by the qobject hierarchy.
     */
    medViewerConfiguration *createConfiguration(QString type,QWidget* parent=0);

protected:
     medViewerConfigurationFactory(void);
    ~medViewerConfigurationFactory(void);

private:
    static medViewerConfigurationFactory *s_instance;

private:
    medViewerConfigurationFactoryPrivate *d;
};

#endif // MEDVIEWERCONFIGURATIONFACTORY_H
