#ifndef MEDVIEWERCONFIGURATIONFACTORY_H
#define MEDVIEWERCONFIGURATIONFACTORY_H

#include "medGuiExport.h"

#include <dtkCore/dtkAbstractFactory.h>

#include <QtCore>

class medViewerConfiguration;
class medViewerConfigurationFactoryPrivate;
struct medViewerConfigurationDetails;

class MEDGUI_EXPORT medViewerConfigurationFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medViewerConfiguration *(*medViewerConfigurationCreator)(QWidget* parent);


public:
    static medViewerConfigurationFactory *instance(void);

    QList<QString> configurations(void);

    /**
     * @brief Registers a medConfiguration type with the factory.
     *
     *
     * This method is templated with the configurationType.
     * This is a convience method.
     * The constructor of the configuration - with a (QWidget *parent) signature -
     * is used to create a function pointer to allocate memory.
     * The configuration source code doesn't need to contain any such
     * function itself.
     *
     * @param identifier Identifier of the type.
     * @param name Human readable name (Potentially localised).
     * @param description short description (Potentially localised).
     */
    template <typename configurationType>
    bool registerConfiguration(QString identifier,
                         QString name,
                         QString description){
        //we must keep the templated part in the .h file for library users
        medViewerConfigurationCreator creator = create<configurationType>;
        return registerConfiguration(identifier,name,description,creator);
    }

    /**
     * @brief Registers a medConfiguration type with the factory.
     *
     * This method requires the developer to provide his own function pointer
     * to allocate the configuration memory.
     *
     * @param identifier Identifier of the type.
     * @param name Human readable name (Potentially localised).
     * @param description short description, mainly used for tooltips
     * (Potentially localised).
     * @param creator function pointer allocating memory for the toolbox.
     */
    bool registerConfiguration(QString identifier,
                         QString name,
                         QString description,
                         medViewerConfigurationCreator creator);

    /**
     * @brief Gives the details of all configurations.
     *
     */
    QHash<QString, medViewerConfigurationDetails *> configurationDetails() const;

    /**
     * @brief Gives the details of one configuration.
     *
     */
    medViewerConfigurationDetails * configurationDetailsFromId(QString identifier) const;

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
    /**
     * @brief Templated method returning a pointer to an allocated configuration.
     * @see template<class configurationType> registerConfiguration
     * @warning keep it static if you don't want to freeze your brain (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */
    template < typename T >
    static medViewerConfiguration* create ( QWidget* parent ) {
    return ( new T(parent) );
    }

private:
    medViewerConfigurationFactoryPrivate *d;
};

/**
 * @brief stores the details for a particular configuration,
 * and a function to allocate memory.
 *
 */
struct MEDGUI_EXPORT medViewerConfigurationDetails{
    QString name; /** Readable name*/
    QString description; /** (tooltip) short description of the configuration */
    medViewerConfigurationFactory::medViewerConfigurationCreator creator; /** function pointer allocating memory for the configuration*/
    medViewerConfigurationDetails(QString name,QString description,
                                  medViewerConfigurationFactory::medViewerConfigurationCreator creator):
        name(name),description(description), creator(creator){}
};

#endif // MEDVIEWERCONFIGURATIONFACTORY_H
