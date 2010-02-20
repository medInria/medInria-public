/* medToolBoxConfiguration.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 22:40:11 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXCONFIGURATION_H
#define MEDTOOLBOXCONFIGURATION_H

#include "medToolBox.h"

class medToolBoxConfigurationPrivate;

class MEDGUI_EXPORT medToolBoxConfiguration : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxConfiguration(QWidget *parent = 0);
    ~medToolBoxConfiguration(void);

    void addConfiguration(QString name);

signals:
    void configurationChanged(QString);

private:
    medToolBoxConfigurationPrivate *d;
};

#endif
