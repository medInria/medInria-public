/* medAbstractPacsFactory.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Nov  7 15:48:10 2008 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 13:16:17 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 52
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#pragma once

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
    typedef medAbstractPacsFindScu *(*medAbstractPacsFindScuCreator)();
    typedef medAbstractPacsEchoScu *(*medAbstractPacsEchoScuCreator)();
    typedef medAbstractPacsMoveScu *(*medAbstractPacsMoveScuCreator)();
    typedef medAbstractPacsStoreScu *(*medAbstractPacsStoreScuCreator)();
    typedef medAbstractPacsStoreScp *(*medAbstractPacsStoreScpCreator)();

    typedef QHash<QString, medAbstractPacsFindScuCreator> medAbstractPacsFindScuCreatorHash;
    typedef QHash<QString, medAbstractPacsEchoScuCreator> medAbstractPacsEchoScuCreatorHash;
    typedef QHash<QString, medAbstractPacsMoveScuCreator> medAbstractPacsMoveScuCreatorHash;
    typedef QHash<QString, medAbstractPacsStoreScuCreator> medAbstractPacsStoreScuCreatorHash;
    typedef QHash<QString, medAbstractPacsStoreScpCreator> medAbstractPacsStoreScpCreatorHash;

public:
    static medAbstractPacsFactory *instance();

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
     medAbstractPacsFactory();
    ~medAbstractPacsFactory();

private:
    static medAbstractPacsFactory *s_instance;

private:
    medAbstractPacsFactoryPrivate *d;
};


