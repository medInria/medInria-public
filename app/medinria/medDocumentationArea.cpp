/* medDocumentationArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 22 09:04:25 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Jun 30 13:06:35 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 38
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
    d->view->setUrl(QUrl("qrc:/html/documentation/index.html"));
    connect(d->view, SIGNAL(linkClicked(const QUrl&)), this, SLOT(linkClicked(const QUrl&)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
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
