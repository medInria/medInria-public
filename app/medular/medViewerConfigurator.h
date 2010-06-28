/* medViewerConfigurator.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 14:13:36 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 14:18:05 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERCONFIGURATOR_H
#define MEDVIEWERCONFIGURATOR_H

#include <QtCore>

class medViewerConfiguration;
class medViewerConfiguratorPrivate;

class medViewerConfigurator : public QObject
{
    Q_OBJECT

public:
    static medViewerConfigurator *instance(void);

    void addConfiguration(QString name, medViewerConfiguration *configuration);

public slots:
    void setConfiguration(QString name);

protected:
     medViewerConfigurator(void);
    ~medViewerConfigurator(void);

protected:
    static medViewerConfigurator *s_instance;

private:
    medViewerConfiguratorPrivate *d;
};

#endif
