/* medClutEditor.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue May  5 12:22:54 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:19:50 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 8
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDCLUTEDITOR_H
#define MEDCLUTEDITOR_H

#include "medGuiExport.h"

#include <QtGui>

class dtkAbstractData;
class dtkAbstractView;

class medClutEditorPrivate;

class MEDGUI_EXPORT medClutEditor : public QWidget
{
    Q_OBJECT

public:
     medClutEditor(QWidget *parent = 0);
    ~medClutEditor(void);

    void setData(dtkAbstractData *data);
    void setView(dtkAbstractView *view);

protected:
    void mousePressEvent(QMouseEvent *event);

protected slots:
    void onNewTableAction(void);
    void onClearTablesAction(void);
    void onApplyTablesAction(void);

private:
    medClutEditorPrivate *d;
};

#endif // MEDCLUTEDITOR_H
