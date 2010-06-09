/* medWelcomeArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct  5 08:29:35 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Mar 19 18:21:00 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 196
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medWelcomeArea.h"
#include <QPropertyAnimation>
#include <QtWebKit>

class medWelcomeAreaPrivate
{
public:
    bool logged;

    QLineEdit *userEdit;
    QLineEdit *passEdit;
    QWebView * medDescription;
    QTextDocument * text;
    QStatusBar *status;
    void setDocument(void);

};

medWelcomeArea::medWelcomeArea(QWidget *parent) : QWidget(parent), d(new medWelcomeAreaPrivate)
{
    d->logged = 0;

    d->userEdit = new QLineEdit(this);
    d->passEdit = new QLineEdit(this);
    d->passEdit->setEchoMode(QLineEdit::Password);

    QFormLayout *form = new QFormLayout;
    form->addRow(    "User", d->userEdit);
    form->addRow("Password", d->passEdit);

    connect(d->passEdit, SIGNAL(returnPressed()), this, SLOT(authenticate()));

    QHBoxLayout *logn = new QHBoxLayout;
    logn->addStretch(1);
    logn->addLayout(form);
    logn->addStretch(1);

    //some description:
    d->medDescription = new QWebView(this);
    //d->medDescription->setAcceptRichText(true);
    //d->medDescription->setReadOnly(true);
    //d->medDescription->setOpenExternalLinks(true);
    d->setDocument();


    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch(1);
    layout->addWidget(d->medDescription);
    layout->addLayout(logn);
    layout->addStretch(1);
}

medWelcomeArea::~medWelcomeArea(void)
{
    delete d;

    d = NULL;
}

void medWelcomeArea::setup(QStatusBar *status)
{
    d->status = status;

    if(!d->logged)
        status->setMaximumHeight(0);
}

void medWelcomeArea::setdw(QStatusBar *status)
{
    d->status = status;
}

void medWelcomeArea::authenticate(void)
{
    QString user = d->userEdit->text();
    QString pass = d->passEdit->text();

    if(!user.isNull() && !pass.isNull()) {
        
        QPropertyAnimation *animation = new QPropertyAnimation(d->status, "maximumHeight");
        animation->setDuration(200);
        animation->setEasingCurve(QEasingCurve::OutCubic);        
        animation->setStartValue(0);
        animation->setEndValue(31);
        animation->start();
        
        connect(animation, SIGNAL(finished()), animation, SLOT(deleteLater()));

        d->logged = true;
    }
}

void medWelcomeArea::linkClicked(const QUrl& url)
{
    Q_UNUSED(url);
}

void medWelcomeAreaPrivate::setDocument()
{
    QString disclaimer (
            "<img src=\":/pixmaps/medular-logo.png\" alt=\"Medular Logo\" />\
            <center>\
            <H1>Medular</H1><H2>Welcome</H2>\
            <center>\
            </br>Welcome to Medular.\
            Medular is a platform developed by INRIA for integrating medical imaging algorithms and tools. \
            </br>\
            Medular is the successor of MedINRIA 1.9, You may find more information on the MedINRIA project at <a href=\"http://www-sop.inria.fr/asclepios/software/MedINRIA/index.php\">http://www-sop.inria.fr/asclepios/software/MedINRIA/index.php</a>.\
            You can download this software and get some additional information at <a href=\"somewhere\">somewhere</a>.\
            ");
    //this->text = new QTextDocument(this->medDescription);
    //text->setHtml(disclaimer);
    //this->medDescription->setDocument(this->text);
    this->medDescription->setHtml((const QString ) disclaimer);
}
