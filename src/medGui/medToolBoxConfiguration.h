/* medToolBoxConfiguration.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 10:39:30 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 4
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

signals:
    void configurationChanged(QString);

private:
    medToolBoxConfigurationPrivate *d;
};

#endif
