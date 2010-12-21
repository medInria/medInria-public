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

#include "medViewerConfiguration.h"

#include "medGuiExport.h"

class medViewerConfigurationVisualizationPrivate;

class MEDGUI_EXPORT medViewerConfigurationVisualization : public medViewerConfiguration
{
    Q_OBJECT

public:
     medViewerConfigurationVisualization(QWidget *parent = 0);
    ~medViewerConfigurationVisualization(void);
    
    virtual QString description(void) const;
        
private:
    medViewerConfigurationVisualizationPrivate *d;
};

MEDGUI_EXPORT medViewerConfiguration *createMedViewerConfigurationVisualization(void);

#endif
