/* medViewerConfiguration.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 14:11:43 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 14:22:37 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERCONFIGURATION_H
#define MEDVIEWERCONFIGURATION_H

#include <QtCore>

class medToolBox;
class medViewerArea;
class medViewerConfigurationPrivate;

class medViewerConfiguration : public QObject
{
    Q_OBJECT

public:
     medViewerConfiguration(medViewerArea *area);
    ~medViewerConfiguration(void);

    void setup(void);
    void setdw(void);

    void attach(int index);
    void attach(medToolBox *toolbox, bool visible);

private:
    medViewerConfigurationPrivate *d;
};

#endif
