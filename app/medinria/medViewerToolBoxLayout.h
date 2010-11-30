/* medViewerToolBoxLayout.h ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Oct  7 13:00:43 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 10
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERTOOLBOXLAYOUT_H
#define MEDVIEWERTOOLBOXLAYOUT_H

#include <medGui/medToolBox.h>

class medViewerToolBoxLayoutPrivate;

class medViewerToolBoxLayout : public medToolBox
{
    Q_OBJECT

public:
     medViewerToolBoxLayout(QWidget *parent = 0);
    ~medViewerToolBoxLayout(void);

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
    medViewerToolBoxLayoutPrivate *d;
};

#endif
