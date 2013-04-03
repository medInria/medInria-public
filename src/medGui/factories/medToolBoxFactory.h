/* medToolBoxFactory.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 11:34:40 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:05:16 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 27
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDTOOLBOXFACTORY_H
#define MEDTOOLBOXFACTORY_H

#include "medGuiExport.h"

#include <dtkCore/dtkAbstractFactory.h>

#include <QtCore>

class medToolBox;
class medToolBoxFactoryPrivate;
class medToolBoxFactory;
struct medToolBoxDetails;

/**
 * @brief Factory for toolbox generation.
 *
 * Any toolbox can be registered with this factory.
 * A toolbox will be identified in the factory by a string that should be unique and identifies the type (in the dtk sense) of the toolbox.
 * It should also have a readable name and description (localised are better).
 *
 * Finally a list of category allows the toolbox to belong to categories such as:
 * <ul>
 * <li> diffusion</li>
 * <li> registration </li>
 * <li> filtering </li>
 * <li> ... </li>
 * </ul>
 * Developers can add their own categories, and use their own filters afterwards.
 *
 * The details for each toolbox are stored in a struct of type medToolBoxDetails.
 */


class MEDGUI_EXPORT medToolBoxFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medToolBox *(*medToolBoxCreator)(QWidget *parent);

public:
    static medToolBoxFactory *instance();
    /**
     * @brief Registers a medToolBox type with the factory.
     *
     *
     * This method is templated with the toolboxType. This is a convience method.
     * The constructor of the toolbox - with a (QWidget *parent) signature -
     * is used to create a function pointer to allocate memory.
     * The toolbox source code doesn't need to contain any such function itself.
     *
     * @param identifier Identifier of the type.
     * @param name Human readable name(Potentially localised).
     * @param description short description, mainly used for tooltips
     * in comboboxes listing the toolboxes (Potentially localised).
     * @param categories List of categories classifying the Toolbox.
     */
    template <typename toolboxType>
    bool registerToolBox(QString identifier,
                         QString name,
                         QString description,
                         QStringList categories){
        //we must keep the templated part in the .h file for library users
        medToolBoxCreator creator = create<toolboxType>;
        return registerToolBox(identifier,name,description,categories,creator);
    }

    /**
     * @brief Registers a medToolBox type with the factory.
     *
     * This method requires the developer to provide his own function pointer
     * to allocate the toolbox memory.
     *
     * @param identifier Identifier of the type.
     * @param name Human readable name(Potentially localised).
     * @param description short description, mainly used for tooltips
     * in comboboxes listing the toolboxes (Potentially localised).
     * @param categories List of categories classifying the Toolbox.
     * @param creator function pointer allocating memory for the toolbox.
     */
    bool registerToolBox(QString identifier,
                         QString name,
                         QString description,
                         QStringList categories,
                         medToolBoxCreator creator);
    /**
     * Get a list of the available toolboxes from a specific category.
     *
     */
    QList<QString> toolBoxesFromCategory(const QString& category) const;


    /**
     * @brief Gets the name, description, categories and creators
     * for the given toolbox.
     */
    medToolBoxDetails* toolBoxDetailsFromId (
            const QString& id )const;

    /**
     * @brief Gets the name, description, categories and creators
     *  of all toolboxes for a given category.
     *
     * Probably slower than using toolBoxesFromCategory() + toolBoxDetailsFromId()
     * but only just, given the probable size of the list.
     */
    QHash<QString, medToolBoxDetails*> toolBoxDetailsFromCategory (
            const QString& id )const;



public slots:
    medToolBox *createToolBox(QString identifier, QWidget *parent=0);


protected:
     medToolBoxFactory();
    ~medToolBoxFactory();

private:
    static medToolBoxFactory *s_instance; /** Singleton holder.*/
    /**
     * @brief Templated method returning a pointer to an allocated toolbox.
     * @see template<class toolboxType> registerToolBox
     * @warning keep it static if you don't want to freeze your brain (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */
    template < typename T >
    static medToolBox* create ( QWidget* parent ) {
    return ( new T(parent) );
    }
private:
    medToolBoxFactoryPrivate *d;
};

/**
 * @brief stores the details for a particular toolbox,
 * and a function to allocate memory.
 *
 */
struct MEDGUI_EXPORT medToolBoxDetails{
    QString name; /** Readable name*/
    QString description; /** (tooltip) short description of the Toolbox */
    QStringList categories; /** List of categories the toolbox falls in*/
    medToolBoxFactory::medToolBoxCreator creator; /** function pointer allocating memory for the toolbox*/
    medToolBoxDetails(QString name,QString description, QStringList categories,
                     medToolBoxFactory::medToolBoxCreator creator):
        name(name),description(description),categories(categories),
        creator(creator){}
};

#endif
