/* itkProcessRegistrationILogDemonsToolBox.h ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 15:39:43 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:28:26 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 22
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include <medAbstractSelectableToolBox.h>

class itkProcessRegistrationILogDemonsToolBoxPrivate;
class medDataIndex;

class itkProcessRegistrationILogDemonsToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("ILog Demons",
                          "interface to the Incompressible Log Demons algorithm",
                          <<"Registration")

public:
     itkProcessRegistrationILogDemonsToolBox(QWidget *parent = 0);
    ~itkProcessRegistrationILogDemonsToolBox(void);

	dtkPlugin * plugin() override;
	medAbstractData * processOutput() override;
    
	static bool registered();
    
public slots:
    void run(void);
    void onMaskDropped(const medDataIndex& index);
    void onUseMaskStateChanged(int state);

private:
    itkProcessRegistrationILogDemonsToolBoxPrivate *d;
};

medAbstractSelectableToolBox *createitkProcessRegistrationILogDemonsToolBox(QWidget * widget);
