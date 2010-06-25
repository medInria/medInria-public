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

#include <dtkCore/dtkGlobal.h>

#include <QtWebKit>

class medWelcomeAreaPrivate
{
public:
    QWebView *view;

    QStatusBar *status;
};

medWelcomeArea::medWelcomeArea(QWidget *parent) : QWidget(parent), d(new medWelcomeAreaPrivate)
{
    d->view= new QWebView(this);
    d->view->setContextMenuPolicy(Qt::NoContextMenu);
    d->view->setAcceptDrops(false);
    d->view->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    d->view->settings()->setAttribute(QWebSettings::JavaEnabled, false);
    d->view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    d->view->setUrl(QUrl("qrc:/html/index.html"));
    connect(d->view, SIGNAL(linkClicked(const QUrl&)), this, SLOT(linkClicked(const QUrl&)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->view);
}

medWelcomeArea::~medWelcomeArea(void)
{
    delete d;

    d = NULL;
}

void medWelcomeArea::setup(QStatusBar *status)
{
    d->status = status;
}

void medWelcomeArea::setdw(QStatusBar *status)
{
    d->status = status;
}

void medWelcomeArea::linkClicked(const QUrl& url)
{    
    if(url.scheme() == "medular") {
        if(url.host() == "browser")
            emit switchToBrowserArea();
        if(url.host() == "viewer")
            emit switchToViewerArea();
        if(url.host() == "documentation"){
            qDebug()<<url.queryItems();
            qDebug()<<url.path();
            emit switchToDocumentationArea(url);
        }
    } else {
        if (url.isRelative())

            d->view->load(url);
        else
            QDesktopServices::openUrl(url);
    }
}

void medWelcomeArea::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::AltModifier && event->key() == Qt::Key_Left)
        d->view->back();

    if(event->modifiers() == Qt::AltModifier && event->key() == Qt::Key_Right)
        d->view->forward();
}
