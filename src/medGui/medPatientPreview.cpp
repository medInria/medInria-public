/* medPatientPreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:46:53 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  8 21:29:48 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 42
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medPatientPreview.h"

#include <medGui/medImageStack.h>

class medPatientPreviewPrivate
{
public:
    medImageStack *stack;
};

medPatientPreview::medPatientPreview(QWidget *parent) : QWidget(parent), d(new medPatientPreviewPrivate)
{
    QLabel *user_avatar = new QLabel(this);
    user_avatar->setPixmap(QPixmap(":/img/unknown.jpg"));

    QLabel *user_name = new QLabel(this);
    user_name->setText("Bernie Noel");

    QVBoxLayout *vertical_layout = new QVBoxLayout;
    vertical_layout->addWidget(user_avatar);
    vertical_layout->addWidget(user_name);

    QWidget *avatar = new QWidget(this);
    avatar->setLayout(vertical_layout);

    d->stack = new medImageStack(this);

    QFormLayout *form_layout = new QFormLayout;
    form_layout->addRow("Patient last name:", new QLabel("Bernie"));
    form_layout->addRow("Patient first name:", new QLabel("Noel"));
    form_layout->addRow("Information 3:", new QLabel("Prout"));
    form_layout->addRow("Information 4:", new QLabel("Prout"));
    form_layout->addRow("Information 5:", new QLabel("Prout"));

    QWidget *information = new QWidget(this);
    information->setLayout(form_layout);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(avatar);
    layout->addWidget(information);
    layout->addWidget(d->stack);
}

medPatientPreview::~medPatientPreview(void)
{
    delete d;

    d = NULL;
}

void medPatientPreview::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(event->rect(), Qt::red);

    QWidget::paintEvent(event);
}
