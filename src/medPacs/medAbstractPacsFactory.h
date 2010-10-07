/* medAbstractPacsFactory.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Nov  7 15:48:10 2008 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 13:07:55 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 51
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
class medAbstractPacsStoreScu;
class medAbstractPacsStoreScp;
class medAbstractPacsFactoryPrivate;

class MEDPACS_EXPORT medAbstractPacsFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medAbstractPacsFindScu *(*medAbstractPacsFindScuCreator)(void);
    typedef medAbstractPacsEchoScu *(*medAbstractPacsEchoScuCreator)(void);
    typedef medAbstractPacsMoveScu *(*medAbstractPacsMoveScuCreator)(void);
    typedef medAbstractPacsStoreScu *(*medAbstractPacsStoreScuCreator) (void);
    typedef medAbstractPacsStoreScp *(*medAbstractPacsStoreScpCreator)(void);

    typedef QHash<QString, medAbstractPacsFindScuCreator> medAbstractPacsFindScuCreatorHash;
    typedef QHash<QString, medAbstractPacsEchoScuCreator> medAbstractPacsEchoScuCreatorHash;
    typedef QHash<QString, medAbstractPacsMoveScuCreator> medAbstractPacsMoveScuCreatorHash;
    typedef QHash<QString, medAbstractPacsStoreScuCreator> medAbstractPacsStoreScuCreatorHash;
    typedef QHash<QString, medAbstractPacsStoreScpCreator> medAbstractPacsStoreScpCreatorHash;

public:
    static medAbstractPacsFactory *instance(void);

    bool registerEchoScuType(QString type, medAbstractPacsEchoScuCreator func);
    bool registerFindScuType(QString type, medAbstractPacsFindScuCreator func);
    bool registerMoveScuType(QString type, medAbstractPacsMoveScuCreator func);
    bool registerStoreScuType(QString type, medAbstractPacsStoreScuCreator func);
    bool registerStoreScpType(QString type, medAbstractPacsStoreScpCreator func);

public slots:
    medAbstractPacsEchoScu *createEchoScu(QString type);
    medAbstractPacsFindScu *createFindScu(QString type);
    medAbstractPacsMoveScu *createMoveScu(QString type);
    medAbstractPacsStoreScu *createStoreScu(QString type);
    medAbstractPacsStoreScp *createStoreScp(QString type);

protected:
     medAbstractPacsFactory(void);
    ~medAbstractPacsFactory(void);

private:
    static medAbstractPacsFactory *s_instance;

private:
    medAbstractPacsFactoryPrivate *d;
};

#endif
