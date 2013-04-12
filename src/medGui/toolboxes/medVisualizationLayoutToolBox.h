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

#pragma once

#include "medGuiExport.h"
#include "medToolBox.h"

class medVisualizationLayoutToolBoxPrivate;

class MEDGUI_EXPORT medVisualizationLayoutToolBox : public medToolBox
{
    Q_OBJECT

public:
     medVisualizationLayoutToolBox(QWidget *parent = 0);
    ~medVisualizationLayoutToolBox();

signals:
    void modeChanged(const QString&);
    void split(int, int);
    void presetClicked(int);

public slots:
    void clear();
    /**
     * @brief Sets the current tab according to container type
     *
     * @param name the container description
    */
    void setTab(const QString &name);

protected slots:
    void onButtonAClicked();
    void onButtonBClicked();
    void onButtonCClicked();
    void onButtonDClicked();
    void onButtonEClicked();
    void modeChanged(int);

private:
    medVisualizationLayoutToolBoxPrivate *d;
};


