/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medToolBox.h>
#include <medToolBoxFactory.h>



class medToolBoxFactoryPrivate
{
public:
    typedef QHash<QString,
        medToolBoxDetails*> medToolBoxCreatorHash;

    medToolBoxCreatorHash creators;

};

medToolBoxFactory *medToolBoxFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medToolBoxFactory;

    return s_instance;
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
bool medToolBoxFactory::registerToolBox(QString identifier,
                                        QString name,
                                        QString description,
                                        QStringList categories,
                                        medToolBoxCreator creator)
{
    if(!d->creators.contains(identifier))
    {
        medToolBoxDetails* holder = new medToolBoxDetails(name,
                                                          description,
                                                          categories,
                                                          creator);
        d->creators.insert( identifier,
                            holder);
        return true;
    }
    return false;
}

/**
 * Get a list of the available toolboxes from a specific category.
 *
 */
QList<QString> medToolBoxFactory::toolBoxesFromCategory(
        const QString& category)const
{
    QList<QString> ids;
    typedef medToolBoxFactoryPrivate::medToolBoxCreatorHash::iterator creator_iterator;
    creator_iterator i = d->creators.begin();
    while (i != d->creators.end())
    {
        if (i.value()->categories.contains(category))
        {
            ids << i.key();
        }
        ++i;
    }
    return ids;
}

medToolBox *medToolBoxFactory::createToolBox(QString identifier,
                                                         QWidget *parent)
{
    if(!d->creators.contains(identifier))
        return NULL;

    medToolBox *toolbox =
            (d->creators[identifier])->creator(parent);

    return toolbox;
}

/**
 * @brief Gets the name, description, categories and creators
 * for the given toolbox.
 */
medToolBoxDetails * medToolBoxFactory::toolBoxDetailsFromId(
        const QString &id) const
{
    return d->creators.value(id);
}


/**
 * @brief Gets the name, description, categories and creators
 *  of all toolboxes for a given category.
 *
 * Probably slower than using toolBoxesFromCategory() + toolBoxDetailsFromId()
 * but only just, given the probable size of the list.
 */
QHash<QString, medToolBoxDetails *> medToolBoxFactory::toolBoxDetailsFromCategory(const QString &cat) const
{
    QHash<QString, medToolBoxDetails *> tbsDetails;
    typedef medToolBoxFactoryPrivate::medToolBoxCreatorHash::iterator creator_iterator;
    creator_iterator i = d->creators.begin();
    while (i != d->creators.end())
    {
        if (i.value()->categories.contains(cat))
        {
            tbsDetails.insert(i.key(),i.value());
        }
        ++i;
    }
    return tbsDetails;
}

medToolBoxFactory::medToolBoxFactory(void) : dtkAbstractFactory(), d(new medToolBoxFactoryPrivate)
{

}

medToolBoxFactory::~medToolBoxFactory(void)
{
    //delete details.
    foreach (medToolBoxDetails * detail, d->creators.values())
    {
        delete detail;
        detail = NULL;
    }

    delete d;

    d = NULL;
}

medToolBoxFactory *medToolBoxFactory::s_instance = NULL;



