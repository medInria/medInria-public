/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <dtkCoreSupport/dtkAbstractFactory.h>

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

class MEDCORE_EXPORT medToolBoxFactory : public dtkAbstractFactory
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
     */
    template <typename toolboxType>
    bool registerToolBox() {
        //we must keep the templated part in the .h file for library users
        medToolBoxCreator creator = create<toolboxType>;
        return registerToolBox(toolboxType::staticIdentifier(),
                               toolboxType::staticName(),
                               toolboxType::staticDescription(),
                               toolboxType::staticCategories(),
                               creator);
    }


    QList<QString> toolBoxesFromCategory(const QString& category) const;
    medToolBoxDetails* toolBoxDetailsFromId ( const QString& id ) const;

    QHash<QString, medToolBoxDetails*> toolBoxDetailsFromCategory (
            const QString& id )const;

public slots:
    medToolBox *createToolBox(QString identifier, QWidget *parent=0);


protected:

    bool registerToolBox(QString identifier,
                         QString name,
                         QString description,
                         QStringList categories,
                         medToolBoxCreator creator);

     medToolBoxFactory();
    ~medToolBoxFactory();

private:
    static medToolBoxFactory *s_instance;
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
struct MEDCORE_EXPORT medToolBoxDetails{
    QString name; /** Readable name*/
    QString description; /** (tooltip) short description of the Toolbox */
    QStringList categories; /** List of categories the toolbox falls in*/
    medToolBoxFactory::medToolBoxCreator creator; /** function pointer allocating memory for the toolbox*/
    medToolBoxDetails(QString name,QString description, QStringList categories,
                     medToolBoxFactory::medToolBoxCreator creator):
        name(name),description(description),categories(categories),
        creator(creator){}
};


