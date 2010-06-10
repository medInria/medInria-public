/* medToolBoxLayout.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri May 21 15:00:09 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXLAYOUT_H
#define MEDTOOLBOXLAYOUT_H

#include "medToolBox.h"

class medToolBoxLayoutPrivate;

class MEDGUI_EXPORT medToolBoxLayout : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxLayout(QWidget *parent = 0);
    ~medToolBoxLayout(void);

signals:
    void modeChanged(int);
    void split(int, int);
    void presetClicked(int);

protected slots:
    void onButtonAClicked(void);
    void onButtonBClicked(void);
    void onButtonCClicked(void);
    void onButtonDClicked(void);
    void onButtonEClicked(void);

private:
    medToolBoxLayoutPrivate *d;
};

#endif
