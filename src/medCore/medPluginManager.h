/* medPluginManager.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct 28 15:36:13 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Oct 28 18:51:57 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 8
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDPLUGINMANAGER_H
#define MEDPLUGINMANAGER_H

#include <QObject>

#include "medCoreExport.h"

class medPluginManagerPrivate;

class MEDCORE_EXPORT medPluginManager : public QObject
{
    Q_OBJECT

public:
    static medPluginManager *instance(void);

    void   initialize(void);
    void uninitialize(void);

    void  readSettings(void);
    void writeSettings(void);

protected:
     medPluginManager(void);
    ~medPluginManager(void);

private:
    static medPluginManager *s_instance;

private:
    medPluginManagerPrivate *d;
};

#endif // MEDPLUGINMANAGER_H
