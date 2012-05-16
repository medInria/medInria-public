/* medToolBoxFactory.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 14:33:05 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:06:00 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 32
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medToolBox.h"
#include "medToolBoxFactory.h"



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
template <class T>
bool medToolBoxFactory::registerToolBox(QString identifier,
                                        QString name,
                                        QString description,
                                        QStringList categories)
{
    if(!d->creators.contains(identifier))
    {
        medToolBoxDetails* holder = new medToolBoxDetails(name,
                                                          description,
                                                          categories,
                                                          create<T>);
        d->creators.insert( identifier,
                            holder);
        return true;
    }
    return false;
}

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

medToolBoxDetails * medToolBoxFactory::toolBoxDetailsFromId(
        const QString &id) const
{
    return d->creators.value(id);
}

medToolBoxFactory::medToolBoxFactory(void) : dtkAbstractFactory(), d(new medToolBoxFactoryPrivate)
{

}

medToolBoxFactory::~medToolBoxFactory(void)
{

    delete d;

    d = NULL;
}

medToolBoxFactory *medToolBoxFactory::s_instance = NULL;


