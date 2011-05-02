/* itkProcessRegistrationDiffeoMorphicDemonsToolBox.h ---
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

#ifndef ITKPROCESSREGISTRATIONDIFFEOMORPHICDEMONSTOOLBOX_H
#define ITKPROCESSREGISTRATIONDIFFEOMORPHICDEMONSTOOLBOX_H

#include <medGui/medToolBoxRegistrationCustom.h>

class itkProcessRegistrationDiffeoMorphicDemonsToolBoxPrivate;

class itkProcessRegistrationDiffeoMorphicDemonsToolBox : public medToolBoxRegistrationCustom
{
    Q_OBJECT

public:
     itkProcessRegistrationDiffeoMorphicDemonsToolBox(QWidget *parent = 0);
    ~itkProcessRegistrationDiffeoMorphicDemonsToolBox(void);

public:
    static bool registered(void);

public slots:
    void run(void);

private:
    itkProcessRegistrationDiffeoMorphicDemonsToolBoxPrivate *d;
};

medToolBoxRegistrationCustom *createItkProcessRegistrationDiffeoMorphicDemonsToolBox(QWidget *parent);

#endif
