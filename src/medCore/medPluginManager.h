/* medPluginManager.h -
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct 28 15:36:13 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb 22 21:22:12 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 15
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDPLUGINMANAGER_H
#define MEDPLUGINMANAGER_H

#include <QtCore>

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

    QStringList handlers(const QString& category);
    void setPath(const QString& path);
     
public slots:
    void onPluginLoaded(const QString& name);

protected:
     medPluginManager(void);
    ~medPluginManager(void);

private:
    static medPluginManager *s_instance;

private:
    medPluginManagerPrivate *d;
};

#endif // MEDPLUGINMANAGER_H
