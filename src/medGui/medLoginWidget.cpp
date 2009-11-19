/* medLoginWidget.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct  5 15:36:10 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct 23 11:34:06 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 161
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
    QPushButton *loginButton;
    QPushButton *adminButton;
    QTimer *timer;

    QString user;
    QString passwd;

    medLoginWidget::State state;
};

medLoginWidget::medLoginWidget(QWidget *parent) : QWidget(parent), d(new medLoginWidgetPrivate)
{
    d->userLabel = new QLabel(this);
    d->loginLabel = new QLabel(this);
    d->loginLabel->setText("Login:");
    d->loginEdit = new QLineEdit(this);
    d->adminButton = new QPushButton("Admin", this);
    d->adminButton->hide();
    d->loginButton = new QPushButton("Logout", this);
    d->loginButton->hide();

    connect(d->loginEdit, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
    connect(d->adminButton, SIGNAL(clicked()), this, SLOT(onAdminButtonClicked()));
    connect(d->loginButton, SIGNAL(clicked()), this, SLOT(onLoginButtonClicked()));

    d->layout = new QHBoxLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(5);
    d->layout->addWidget(d->userLabel);
    d->layout->addStretch(1);
    d->layout->addWidget(d->loginLabel);
    d->layout->addWidget(d->loginEdit);
    d->layout->addWidget(d->adminButton);
    d->layout->addWidget(d->loginButton);

    d->timer = new QTimer;
    d->timer->start(1000);
    connect(d->timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));

    d->state = NONE;
}

medLoginWidget::~medLoginWidget(void)
{
    delete d;

    d = NULL;
}

void medLoginWidget::onTimeOut(void)
{
    d->userLabel->setText(QString("Guest | ") + QDateTime::currentDateTime().toString());
}

void medLoginWidget::onReturnPressed(void)
{
    if(d->state == NONE) {

        QString login = d->loginEdit->text();
        
        if(login == "admin") {
            d->loginEdit->clear();
            d->loginEdit->setEchoMode(QLineEdit::Password);
            d->loginLabel->setText("Password:");
            d->state = LOGGING;

            d->user = login;
        }

    } else if(d->state == LOGGING) {
        
        QString password = d->loginEdit->text();
        
        if(password == "passwd") {
            d->loginEdit->clear();
            d->loginEdit->setEchoMode(QLineEdit::Normal);
            d->loginLabel->setText("Login:");
            d->state = LOGGED;

            emit accepted();

            d->loginLabel->setText("Logged as " + d->user);
            d->loginEdit->hide();
            d->adminButton->show();
            d->loginButton->show();

        } else {
            d->loginEdit->clear();
            d->loginEdit->setEchoMode(QLineEdit::Normal);
            d->loginLabel->setText("Login:");
            d->state = NONE;
            emit rejected();
        }
    }
}

void medLoginWidget::onLoginButtonClicked(void)
{
    if(d->state == LOGGED) {
        d->user.clear();
        d->loginLabel->setText("Login:");
        d->loginEdit->show();
        d->adminButton->hide();
        d->loginButton->hide();
        d->state = NONE;
    }
}

void medLoginWidget::onAdminButtonClicked(void)
{
    if(d->state == LOGGED)
        emit accepted();
}
