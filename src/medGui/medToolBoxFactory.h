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
class medToolBoxRegistrationCustom;
class medToolBoxDiffusionCustom;
class medToolBoxSourceData;

class MEDGUI_EXPORT medToolBoxFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medToolBoxRegistrationCustom *(*medToolBoxRegistrationCustomCreator)(void);
    typedef medToolBoxDiffusionCustom *(*medToolBoxDiffusionCustomCreator)(void);
    typedef medToolBoxSourceData *(*medToolBoxSourceDataCreator)(void);

    typedef QHash<QString, medToolBoxRegistrationCustomCreator> medToolBoxRegistrationCustomCreatorHash;
    typedef QHash<QString, medToolBoxDiffusionCustomCreator> medToolBoxDiffusionCustomCreatorHash;
    typedef QHash<QString, medToolBoxSourceDataCreator> medToolBoxSourceDataCreatorHash;

public:
    static medToolBoxFactory *instance(void);

    bool registerCustomRegistrationToolBox(QString type, medToolBoxRegistrationCustomCreator func);
    bool registerCustomDiffusionToolBox(QString type, medToolBoxDiffusionCustomCreator func);
    bool registerSourceDataToolBox(QString type, medToolBoxSourceDataCreator func);

    QList<QString> registrationToolBoxes(void);
    QList<QString> diffusionToolBoxes(void);
    QList<QString> sourcedataToolBoxes(void);

public slots:
    medToolBoxRegistrationCustom *createCustomRegistrationToolBox(QString type);
    medToolBoxDiffusionCustom *createCustomDiffusionToolBox(QString type);
    medToolBoxSourceData *createSourceDataToolBox(QString type);

protected:
     medToolBoxFactory(void);
    ~medToolBoxFactory(void);

private:
    static medToolBoxFactory *s_instance;

private:
    medToolBoxFactoryPrivate *d;
};

#endif
