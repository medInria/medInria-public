/* medToolBox.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 19:41:48 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 12:51:04 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 83
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOX_H
#define MEDTOOLBOX_H

#include "medGuiExport.h"

#include <QtGui>

class dtkAbstractView;

class medToolBoxTab;
class medToolBoxPrivate;

class MEDGUI_EXPORT medToolBox : public QWidget
{
    Q_OBJECT

public:
             medToolBox(QWidget *parent = 0);
    virtual ~medToolBox(void);

    void addWidget(QWidget *widget);

    void setTabWidget (medToolBoxTab* tab);
    void setTitle(const QString& title);
    void setOrientation(Qt::Orientation orientation);    

    Qt::Orientation orientation(void) const;

signals:
    void    addToolBox(medToolBox *toolbox);
    void removeToolBox(medToolBox *toolbox);
    
    void success(void);
    void failure(void);

public slots:
    virtual void clear(void);
    virtual void update(dtkAbstractView *view);

private:
    medToolBoxPrivate *d;
};

#endif
