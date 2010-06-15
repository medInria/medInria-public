/* medDocumentationArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 22 09:04:25 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Mar 19 18:45:06 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 33
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDocumentationArea.h"
#include <QtWebKit>

// /////////////////////////////////////////////////////////////////
// medDocumentationArea
// /////////////////////////////////////////////////////////////////

class medDocumentationAreaPrivate
{
public:
    QStatusBar *status;
    QWebView *view;
    QPushButton *previousPushButton;
    QPushButton *nextPushButton;

};

medDocumentationArea::medDocumentationArea(QWidget *parent) : QWidget(parent), d(new medDocumentationAreaPrivate)
{
    d->view= new QWebView(this);
    d->view->setContextMenuPolicy(Qt::NoContextMenu);
    d->view->setAcceptDrops(false);
    d->view->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    d->view->settings()->setAttribute(QWebSettings::JavaEnabled, false);
    d->view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    d->view->setUrl(QUrl("qrc:/html/doc/index.html"));
    connect(d->view, SIGNAL(linkClicked(const QUrl&)), this, SLOT(linkClicked(const QUrl&)));


    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setAlignment(Qt::AlignLeft);
    hLayout->setContentsMargins(10,10,10,10);
    hLayout->setSpacing(10);
    d->previousPushButton = new QPushButton("Previous", this);
    d->previousPushButton->setIcon (QIcon (":/pixmaps/documentation_previous_arrow.png"));

    d->nextPushButton = new QPushButton("Next", this);
    d->nextPushButton->setIcon (QIcon (":/pixmaps/documentation_next_arrow.png"));

    connect(d->previousPushButton, SIGNAL(clicked(bool)), d->view, SLOT(back()));
    connect(d->nextPushButton, SIGNAL(clicked(bool)), d->view, SLOT(forward()));

    hLayout->addWidget(d->previousPushButton);
    hLayout->addWidget(d->nextPushButton);


    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addLayout(hLayout);
    layout->addWidget(d->view);


}

medDocumentationArea::~medDocumentationArea(void)
{
    delete d;

    d = NULL;
}

void medDocumentationArea::setup(QStatusBar *status)
{
    d->status = status;
}

void medDocumentationArea::setdw(QStatusBar *status)
{
    d->status = status;
}

void medDocumentationArea::loadDoc(const QUrl &url){
    QString path="qrc:/html/doc";
    path+=url.path();
    d->view->load(QUrl(path));
}

void medDocumentationArea::linkClicked(const QUrl& url)
{
    d->view->load(url);
}
void medDocumentationArea::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::AltModifier && event->key() == Qt::Key_Left)
        d->view->back();

    if(event->modifiers() == Qt::AltModifier && event->key() == Qt::Key_Right)
        d->view->forward();
}
