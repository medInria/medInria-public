/* medViewerToolBoxConfiguration.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:58:10 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 8
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERTOOLBOXCONFIGURATION_H
#define MEDVIEWERTOOLBOXCONFIGURATION_H

#include <medGui/medToolBox.h>

class medViewerToolBoxConfigurationPrivate;

class MEDGUI_EXPORT medViewerToolBoxConfiguration : public medToolBox
{
    Q_OBJECT

public:
     medViewerToolBoxConfiguration(QWidget *parent = 0);
    ~medViewerToolBoxConfiguration(void);

    void addConfiguration(QString name);

signals:
    void configurationChanged(QString);

private:
    medViewerToolBoxConfigurationPrivate *d;
};

#endif
