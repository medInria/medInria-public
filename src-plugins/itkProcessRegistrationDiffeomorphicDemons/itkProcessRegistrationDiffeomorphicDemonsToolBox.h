/* itkProcessRegistrationDiffeomorphicDemonsToolBox.h ---
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

#include <medRegistrationAbstractToolBox.h>

class itkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate;

class itkProcessRegistrationDiffeomorphicDemonsToolBox : public medRegistrationAbstractToolBox
{
    Q_OBJECT

public:
     itkProcessRegistrationDiffeomorphicDemonsToolBox(QWidget *parentToolBox = 0);
    ~itkProcessRegistrationDiffeomorphicDemonsToolBox();

public:
    static bool registered();

public slots:
    void run();

private:
    itkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate *d;
};



