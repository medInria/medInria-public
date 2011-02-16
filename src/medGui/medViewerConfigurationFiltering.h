/* medViewerConfigurationFiltering.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 14:12:19 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 11:04:35 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 7
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef medViewerConfigurationFiltering_H
#define medViewerConfigurationFiltering_H

#include <QtCore>

#include "medViewerConfiguration.h"

#include "medGuiExport.h"

class medViewerConfigurationFilteringPrivate;
class medViewContainerStack;

class MEDGUI_EXPORT medViewerConfigurationFiltering : public medViewerConfiguration
{
    Q_OBJECT

public:
     medViewerConfigurationFiltering(QWidget *parent = 0);
    ~medViewerConfigurationFiltering(void);
    
    virtual QString description(void) const;
    
    void setupViewContainerStack (medViewContainerStack *container);

public slots:    
    void patientChanged(int patientId);
    void onProcessSuccess(void);

private:
    medViewerConfigurationFilteringPrivate *d;
};

MEDGUI_EXPORT medViewerConfiguration *createMedViewerConfigurationFiltering(void);

#endif
