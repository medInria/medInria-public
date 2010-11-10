/* medToolBoxFactory.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 11:34:40 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 14:43:47 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 23
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
class medToolBoxRegistrationCustom;

class MEDGUI_EXPORT medToolBoxFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medToolBoxRegistrationCustom *(*medToolBoxRegistrationCustomCreator)(void);

    typedef QHash<QString, medToolBoxRegistrationCustomCreator> medToolBoxRegistrationCustomCreatorHash;

public:
    static medToolBoxFactory *instance(void);

    bool registerCustomRegistrationToolBox(QString type, medToolBoxRegistrationCustomCreator func);

public slots:
    medToolBoxRegistrationCustom *createCustomRegistrationToolBox(QString type);

protected:
     medToolBoxFactory(void);
    ~medToolBoxFactory(void);

private:
    static medToolBoxFactory *s_instance;

private:
    medToolBoxFactoryPrivate *d;
};

#endif
