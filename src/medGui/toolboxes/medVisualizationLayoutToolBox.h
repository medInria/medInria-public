/* medVisualizationLayoutToolBox.h ---
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

#include "medGuiExport.h"
#include "medToolBox.h"

class medVisualizationLayoutToolBoxPrivate;

class MEDGUI_EXPORT medVisualizationLayoutToolBox : public medToolBox
{
    Q_OBJECT

public:
     medVisualizationLayoutToolBox(QWidget *parent = 0);
    ~medVisualizationLayoutToolBox(void);

signals:
    void modeChanged(const QString&);
    void split(int, int);
    void presetClicked(int);

public slots:
    void clear(void);
    /**
     * @brief Sets the current tab according to container type
     *
     * @param name the container description
    */
    void setTab(const QString &name);

protected slots:
    void onButtonAClicked(void);
    void onButtonBClicked(void);
    void onButtonCClicked(void);
    void onButtonDClicked(void);
    void onButtonEClicked(void);
    void modeChanged(int);

private:
    medVisualizationLayoutToolBoxPrivate *d;
};

#endif
