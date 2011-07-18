/* medViewerConfigurationVisualization.h --- 
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

#ifndef medViewerConfigurationVisualization_H
#define medViewerConfigurationVisualization_H

#include <QtCore>

#include <medViewerConfiguration.h>


class medViewerConfigurationVisualizationPrivate;

class medViewerConfigurationVisualization : public medViewerConfiguration
{
    Q_OBJECT

public:
     medViewerConfigurationVisualization(QWidget *parent = 0);
    ~medViewerConfigurationVisualization(void);
    virtual void setupViewContainerStack();
    virtual QString description(void) const;
        
private:
    medViewerConfigurationVisualizationPrivate *d;
};

medViewerConfiguration *createMedViewerConfigurationVisualization(void);

#endif
