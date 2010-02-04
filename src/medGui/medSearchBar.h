/* medSearchBar.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Dec  9 10:18:59 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Feb  4 10:49:02 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 2
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDSEARCHBAR_H
#define MEDSEARCHBAR_H

#include "medGuiExport.h"

#include <QtGui>

class medSearchBarItemPrivate;

class MEDGUI_EXPORT medSearchBarItem : public QFrame
{
    Q_OBJECT

public:
     medSearchBarItem(QWidget *parent);
    ~medSearchBarItem(void);

    void  addKey(const QString& key);

    QString key(void) const;
    QString value(void) const;

    QSortFilterProxyModel *proxy(void);

signals:
    void more(medSearchBarItem *item);
    void less(medSearchBarItem *item);

    void   keyChanged(QString);
    void valueChanged(QString);

public slots:
    void   setKey(const QString& key);
    void setValue(const QString& value);

protected slots:
    void onMoreClicked(void);
    void onLessClicked(void);

private slots:
    void setFilterKeyColumn(int column);

private:
    medSearchBarItemPrivate *d;
};

class medSearchBarPrivate;

class MEDGUI_EXPORT medSearchBar : public QWidget
{
    Q_OBJECT

public:
     medSearchBar(QWidget *parent = 0);
    ~medSearchBar(void);

    void        addKey(const QString& key);
    void setDefaultKey(const QString& key);

    void setModel(QAbstractItemModel *model);
    void setView(QAbstractItemView *view);

protected slots:
    void    addItem(medSearchBarItem *item);
    void removeItem(medSearchBarItem *item);

    void setup(QString value);

private slots:
    void filter(void);

private:
    medSearchBarPrivate *d;
};

#endif // MEDSEARCHBAR_H
