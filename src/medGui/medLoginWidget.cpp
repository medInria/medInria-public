/* medLoginWidget.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct  5 15:36:10 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct 15 15:21:06 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 109
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
    QLabel *userLabel;
    QLabel *loginLabel;
    QLineEdit *loginEdit;
    QTimer *timer;

    int state;
};

medLoginWidget::medLoginWidget(QWidget *parent) : QWidget(parent), d(new medLoginWidgetPrivate)
{
    // QWidget *main = new QWidget(this);

    d->userLabel = new QLabel(this);
    d->loginLabel = new QLabel(this);
    d->loginLabel->setText("Login:");
    d->loginEdit = new QLineEdit(this);

    connect(d->loginEdit, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));

    d->layout = new QHBoxLayout(this);
    d->layout->addWidget(d->userLabel);
    d->layout->addStretch(1);
    d->layout->addWidget(d->loginLabel);
    d->layout->addWidget(d->loginEdit);

    d->timer = new QTimer;
    d->timer->start(1000);
    connect(d->timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));

    // { // this widget layout
    // QVBoxLayout *layout = new QVBoxLayout(this);
    // layout->setContentsMargins(0, 0, 0, 0);
    // layout->setSpacing(0);
    // layout->addWidget(this);
    // }

    d->state = 0;
}

medLoginWidget::~medLoginWidget(void)
{

}

void medLoginWidget::onTimeOut(void)
{
    d->userLabel->setText(QString("Guest | ") + QDateTime::currentDateTime().toString());
}

void medLoginWidget::onReturnPressed(void)
{
    if(d->state == 0) {

        QString login = d->loginEdit->text();
        
        qDebug() << __func__ << login;
        
        if(!login.isEmpty()) {
            d->loginEdit->clear();
            d->loginEdit->setEchoMode(QLineEdit::Password);
            d->loginLabel->setText("Password:");
            d->state = 1;
        }

    } else {
        
        QString password = d->loginEdit->text();
        
        qDebug() << __func__ << password;
        
        if(!password.isEmpty()) {
            d->loginEdit->clear();
            d->loginEdit->setEchoMode(QLineEdit::Normal);
            d->loginLabel->setText("Login:");
            d->state = 0;
        }
    }
}

void medLoginWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setOpacity(0.5);
    painter.fillRect(event->rect(), this->palette().color(QPalette::Window));
    painter.end();

    QWidget::paintEvent(event);
}
