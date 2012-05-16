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

class MEDGUI_EXPORT medToolBoxFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medToolBox *(*medToolBoxCreator)(QWidget *parent);

public:
    static medToolBoxFactory *instance(void);
    template <class toolboxType>
    bool registerToolBox(QString identifier,
                                           QString name,
                                           QString description,
                                           QStringList categories);

    /**
     * Get a list of the available toolboxes from a specific category.
     *
     */
    QList<QString> toolBoxesFromCategory(const QString& category) const;


    /**
     *Get the id, description pair for the given toolbox.
     *
     */
    medToolBoxDetails* toolBoxDetailsFromId (
            const QString& id )const;


public slots:
    medToolBox *createToolBox(QString identifier, QWidget *parent=0);


protected:
     medToolBoxFactory(void);
    ~medToolBoxFactory(void);

private:
    static medToolBoxFactory *s_instance;
    template < typename T >
    medToolBox* create ( QWidget* parent ) {
    return ( new T(parent) );
    }
private:
    medToolBoxFactoryPrivate *d;
};

struct MEDGUI_EXPORT medToolBoxDetails{
    QString name;
    QString description;
    QStringList categories;
    medToolBoxFactory::medToolBoxCreator creator;
    medToolBoxDetails(QString name,QString description, QStringList categories,
                     medToolBoxFactory::medToolBoxCreator creator):
        name(name),description(description),categories(categories),
        creator(creator){}
};

#endif
