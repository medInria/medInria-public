/* medSearchBar.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Dec  9 10:18:59 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Feb  4 10:49:04 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 2
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medSearchBar.h"
#include "medSearchBox.h"

class medSearchBarItemPrivate
{
public:
    QComboBox *key;
    QLineEdit *value;

    QPushButton *more;
    QPushButton *less;

    QSortFilterProxyModel *proxy;
};

medSearchBarItem::medSearchBarItem(QWidget *parent) : QFrame(parent), d(new medSearchBarItemPrivate)
{
    d->key = new QComboBox(this);
    d->value = new QLineEdit(this);
    d->more = new QPushButton("+", this);
    d->less = new QPushButton("-", this);

    d->proxy = new QSortFilterProxyModel(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->key);
    layout->addWidget(d->value);
    layout->addWidget(d->more);
    layout->addWidget(d->less);

    connect(d->more, SIGNAL(clicked()), this, SLOT(onMoreClicked()));
    connect(d->less, SIGNAL(clicked()), this, SLOT(onLessClicked()));
    connect(d->key, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(keyChanged(QString)));
    connect(d->key, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilterKeyColumn(int)));
    connect(d->value, SIGNAL(textChanged(QString)), this, SIGNAL(valueChanged(QString)));
    connect(d->value, SIGNAL(textChanged(QString)), d->proxy, SLOT(setFilterRegExp(QString)));

    this->setAttribute(Qt::WA_MacShowFocusRect, false);
}

medSearchBarItem::~medSearchBarItem(void)
{
    delete d;

    d = NULL;
}

void medSearchBarItem::addKey(const QString& key)
{
    d->key->addItem(key);
}

void medSearchBarItem::setKey(const QString& key)
{
    d->key->setCurrentIndex(d->key->findText(key));
}

void medSearchBarItem::setValue(const QString& value)
{
    d->value->setText(value);
}

QString medSearchBarItem::key(void) const
{
    return d->key->currentText();
}

QString medSearchBarItem::value(void) const
{
    return d->value->text();
}

QSortFilterProxyModel *medSearchBarItem::proxy(void)
{
    return d->proxy;
}

void medSearchBarItem::onMoreClicked(void)
{
    emit more(this);
}

void medSearchBarItem::onLessClicked(void)
{
    emit less(this);
}

void medSearchBarItem::setFilterKeyColumn(int column)
{
    d->proxy->setFilterKeyColumn(column);
}

class medSearchBarPrivate
{
public:

    // -- graphical interface

    QVBoxLayout *layout;
    medSearchBox *box;

    QStringList keys;
    QString default_key;

    // -- logical interface

    QAbstractItemModel *model;
    QAbstractItemView *view;
};

medSearchBar::medSearchBar(QWidget *parent) : QWidget(parent), d(new medSearchBarPrivate)
{
    d->model = NULL;
    d->view = NULL;

    d->box = new medSearchBox(this);

    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    d->layout->addWidget(d->box);

    connect(d->box, SIGNAL(search(QString)), this, SLOT(setup(QString)));
}

medSearchBar::~medSearchBar(void)
{
    delete d;

    d = NULL;
}

void medSearchBar::addKey(const QString& key)
{
    d->keys << key;
}

void medSearchBar::setDefaultKey(const QString& key)
{
    d->default_key = key;
}

void medSearchBar::setModel(QAbstractItemModel *model)
{
    d->model = model;
}

void medSearchBar::setView(QAbstractItemView *view)
{
    d->view = view;
}

void medSearchBar::setup(QString value)
{
    this->addItem(NULL);
}

void medSearchBar::addItem(medSearchBarItem *item)
{
    Q_UNUSED(item);

    medSearchBarItem *new_item = new medSearchBarItem(this);

    if(d->layout->count() == 1) {

        new_item->addKey(d->default_key);
        new_item->setKey(d->default_key);
        new_item->setValue(d->box->text());

        connect(new_item, SIGNAL(valueChanged(QString)), d->box, SLOT(setText(QString)));
        connect(d->box, SIGNAL(textChanged(QString)), new_item, SLOT(setValue(QString)));

    }  else {

        foreach(QString key, d->keys)
            new_item->addKey(key);

    }

    connect(new_item, SIGNAL(more(medSearchBarItem *)), this, SLOT(addItem(medSearchBarItem *)));
    connect(new_item, SIGNAL(less(medSearchBarItem *)), this, SLOT(removeItem(medSearchBarItem *)));

    d->layout->addWidget(new_item);

    filter();
}

void medSearchBar::removeItem(medSearchBarItem *item)
{
    if(d->layout->count() == 1)
        return;

    d->layout->removeWidget(item);

    delete item;

    filter();
}

void medSearchBar::filter(void)
{
    if(!d->model && !d->view)
        return;

    QSortFilterProxyModel *last = NULL;

    for(int i = 1; i < d->layout->count(); i++) {

        medSearchBarItem *item = dynamic_cast<medSearchBarItem *>(d->layout->itemAt(i)->widget());

        last ? item->proxy()->setSourceModel(last) : item->proxy()->setSourceModel(d->model);
        last = item->proxy();
    }

    if(last)
        d->view->setModel(last);
    else
        d->view->setModel(d->model);
}
