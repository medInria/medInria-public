/* medSearchBox.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Dec  9 10:07:32 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Feb  4 10:49:32 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 14
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include <medGui/medSearchBox.h>

// /////////////////////////////////////////////////////////////////
// medSearchBoxPrivate
// /////////////////////////////////////////////////////////////////

class medSearchBoxPrivate
{
public:
    QLineEdit *edit;
    QPushButton *button;
};

// /////////////////////////////////////////////////////////////////
// medSearchBox
// /////////////////////////////////////////////////////////////////

medSearchBox::medSearchBox(QWidget *parent) : QWidget(parent), d(new medSearchBoxPrivate)
{
    d->edit = new QLineEdit(this);
    d->button = new QPushButton("Search", this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 0, 0, 0);
    layout->setSpacing(5);
    layout->addWidget(d->edit);
    layout->addWidget(d->button);

    connect(d->edit, SIGNAL(returnPressed()), this, SLOT(onEnterPressed()));
    connect(d->edit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));
    connect(d->button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
}

medSearchBox::~medSearchBox(void)
{
    delete d;

    d = NULL;
}

QString medSearchBox::text(void) const
{
    return d->edit->text();
}

void medSearchBox::setText(const QString& text)
{
    d->edit->setText(text);
}

void medSearchBox::onButtonClicked(void)
{
    emit search(d->edit->text());
}

void medSearchBox::onEnterPressed(void)
{
    emit search(d->edit->text());
}
