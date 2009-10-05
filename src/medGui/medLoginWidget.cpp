/* medLoginWidget.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct  5 15:36:10 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Oct  5 16:04:58 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 36
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medLoginWidget.h"

class medLoginWidgetPrivate
{
public:
    QHBoxLayout *layout;
    QLineEdit *lineEdit;
    QLabel *userLabel;
    QTimer *timer;
};

medLoginWidget::medLoginWidget(QWidget *parent) : QWidget(parent), d(new medLoginWidgetPrivate)
{
    d->userLabel = new QLabel(this);

    d->lineEdit = new QLineEdit(this);

    d->layout = new QHBoxLayout(this);
    d->layout->addWidget(d->userLabel);
    d->layout->addStretch(1);
    d->layout->addWidget(d->lineEdit);

    d->timer = new QTimer;
    d->timer->start(1000);
    connect(d->timer, SIGNAL(timeout()), this, SLOT(update()));

    this->setStyleSheet(
    "background-color: qrgba(0, 0, 0, 0.5);"
    );
}

medLoginWidget::~medLoginWidget(void)
{

}

void medLoginWidget::update(void)
{
    d->userLabel->setText(QString("Guest | ") + QDateTime::currentDateTime().toString());
}
