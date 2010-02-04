/* medSearchBox.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  7 08:14:03 2008 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Feb  4 10:49:34 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 63
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDSEARCHBOX_H
#define MEDSEARCHBOX_H

#include <QtGui>

#include <medGui/medGuiExport.h>

class medSearchBoxPrivate;

class MEDGUI_EXPORT medSearchBox : public QWidget
{
    Q_OBJECT

public:
    medSearchBox(QWidget *parent = 0);
   ~medSearchBox(void);

   QString text(void) const;

signals:
   void search(const QString& text);
   void textChanged(const QString& text);

public slots:
    void setText(const QString& text);

private slots:
   void onButtonClicked(void);
   void onEnterPressed(void);

private:
   medSearchBoxPrivate *d;
};

#endif
