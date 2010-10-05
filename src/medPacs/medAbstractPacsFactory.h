/* medAbstractPacsFactory.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Nov  7 15:48:10 2008 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Sep 11 13:33:02 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 48
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDABSTRACTPACSFACTORY_H
#define MEDABSTRACTPACSFACTORY_H

#include "medPacsExport.h"

#include <dtkCore/dtkAbstractFactory.h>

#include <QtCore>

class medAbstractPacsEchoScu;
class medAbstractPacsFindScu;
class medAbstractPacsMoveScu;
class medAbstractPacsFactoryPrivate;

class MEDPACS_EXPORT medAbstractPacsFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medAbstractPacsFindScu *(*medAbstractPacsFindScuCreator)(void);
    typedef medAbstractPacsEchoScu *(*medAbstractPacsEchoScuCreator)(void);
    typedef medAbstractPacsMoveScu *(*medAbstractPacsMoveScuCreator)(void);

    typedef QHash<QString, medAbstractPacsFindScuCreator> medAbstractPacsFindScuCreatorHash;
    typedef QHash<QString, medAbstractPacsEchoScuCreator> medAbstractPacsEchoScuCreatorHash;
    typedef QHash<QString, medAbstractPacsMoveScuCreator> medAbstractPacsMoveScuCreatorHash;

public:
    static medAbstractPacsFactory *instance(void);

    bool registerEchoScuType(QString type, medAbstractPacsEchoScuCreator func);

    bool registerFindScuType(QString type, medAbstractPacsFindScuCreator func);

    bool registerMoveScuType(QString type, medAbstractPacsMoveScuCreator func);

public slots:

    medAbstractPacsEchoScu *createEchoScu(QString type);

    medAbstractPacsFindScu *createFindScu(QString type);

    medAbstractPacsMoveScu *createMoveScu(QString type);

protected:
     medAbstractPacsFactory(void);
    ~medAbstractPacsFactory(void);

private:
    static medAbstractPacsFactory *s_instance;

private:
    medAbstractPacsFactoryPrivate *d;
};

#endif
