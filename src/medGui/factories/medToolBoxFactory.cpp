/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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



