/* medDocumentationArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 22 09:04:25 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Feb 11 14:44:59 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 32
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDocumentationArea.h"

#include <dtkGui/dtkAddressBar.h>
#include <dtkGui/dtkNavigationBar.h>
#include <dtkGui/dtkRegistrationBar.h>
#include <dtkGui/dtkSearchBar.h>
#include <dtkGui/dtkSplitter.h>

#include <dtkHelp/dtkHelpController.h>
#include <dtkHelp/dtkHelpBrowser.h>

// /////////////////////////////////////////////////////////////////
// medDocumentationArea
// /////////////////////////////////////////////////////////////////

class medDocumentationAreaPrivate
{
public:
    dtkHelpBrowser *browser;

    dtkNavigationBar *navigation;
    dtkRegistrationBar *registration;
    dtkAddressBar *address;
    dtkSearchBar *search;

    QAction *backwardAction;
    QAction *forwardAction;

    QAction   *registerAction;
    QAction *unregisterAction;

    QAction *addressFocusAction;
    QAction *searchFocusAction;

    dtkSplitter *hsplitter;
    dtkSplitter *vsplitter;

    QStatusBar *status;
};

medDocumentationArea::medDocumentationArea(QWidget *parent) : QWidget(parent), d(new medDocumentationAreaPrivate)
{
    d->browser = new dtkHelpBrowser(this);

    connect(d->browser, SIGNAL(sourceChanged(const QUrl&)), this, SLOT(onUrlChanged(const QUrl&)));

    connect(dtkHelpController::instance()->engine()->contentWidget(), SIGNAL(linkActivated(const QUrl&)), d->browser, SLOT(setSource(const QUrl&)));
    connect(dtkHelpController::instance()->engine()->indexWidget(), SIGNAL(linkActivated(const QUrl&, const QString&)), d->browser, SLOT(setSource(const QUrl&)));
    connect(dtkHelpController::instance()->engine()->indexWidget(), SIGNAL(linksActivated(const QMap<QString, QUrl>&, const QString&)), this, SLOT(onLinksActivated(const QMap<QString, QUrl>&, const QString&)));

    d->backwardAction = d->browser->backwardAction();

    d->forwardAction = d->browser->forwardAction();

    d->registerAction = new QAction("Register", this);
    connect(d->registerAction, SIGNAL(triggered()), this, SLOT(onRegisterClicked()));

    d->unregisterAction = new QAction("Unregister", this);
    connect(d->unregisterAction, SIGNAL(triggered()), this, SLOT(onUnregisterClicked()));

    d->navigation = new dtkNavigationBar(this);
    d->navigation->backwardButton()->setShortcut(Qt::ControlModifier + Qt::Key_Left);
    d->navigation->forwardButton()->setShortcut(Qt::ControlModifier + Qt::Key_Right);

    connect(d->navigation->backwardButton(), SIGNAL(clicked()), d->backwardAction, SLOT(trigger()));
    connect(d->navigation->forwardButton(), SIGNAL(clicked()), d->forwardAction, SLOT(trigger()));

    d->registration = new dtkRegistrationBar(this);
    
    connect(d->registration->registerButton(), SIGNAL(clicked()), d->registerAction, SLOT(trigger()));
    connect(d->registration->unregisterButton(), SIGNAL(clicked()), d->unregisterAction, SLOT(trigger()));

    d->address = new dtkAddressBar(this);
    d->address->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(d->address, SIGNAL(addressEntered(const QUrl&)), d->browser, SLOT(setSource(const QUrl&)));

    d->search = new dtkSearchBar(this);
    d->search->setFixedWidth(200);

    connect(d->search, SIGNAL(textChanged(QString)), dtkHelpController::instance()->engine()->indexWidget(), SLOT(filterIndices(QString)));
    connect(d->search, SIGNAL(returnPressed()), dtkHelpController::instance()->engine()->indexWidget(), SLOT(activateCurrentItem()));

    QAction *addressFocusAction = new QAction(this);
    addressFocusAction->setShortcut(Qt::ControlModifier + Qt::Key_L);
    connect(addressFocusAction, SIGNAL(triggered()), this, SLOT(onAddressFocusTriggered()));
    this->addAction(addressFocusAction);

    QAction *searchFocusAction = new QAction(this);
#if defined(Q_WS_MAC)
    searchFocusAction->setShortcut(Qt::ControlModifier + Qt::AltModifier + Qt::Key_F);
#else
    searchFocusAction->setShortcut(Qt::ControlModifier + Qt::Key_K);
#endif
    connect(searchFocusAction, SIGNAL(triggered()), this, SLOT(onSearchFocusTriggered()));
    this->addAction(searchFocusAction);

    d->vsplitter = new dtkSplitter(this, true);
    d->vsplitter->setOrientation(Qt::Vertical);
    d->vsplitter->addWidget(dtkHelpController::instance()->engine()->contentWidget());
    d->vsplitter->addWidget(dtkHelpController::instance()->engine()->indexWidget());

    d->hsplitter = new dtkSplitter(this);
    d->hsplitter->setOrientation(Qt::Horizontal);
    d->hsplitter->addWidget(d->vsplitter);
    d->hsplitter->addWidget(d->browser);

    d->status = NULL;

    d->browser->setSource(QUrl("qthelp://fr.inria.med/doc/index.html"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->hsplitter);
}

medDocumentationArea::~medDocumentationArea(void)
{
    delete d;

    d = NULL;
}

void medDocumentationArea::setup(QStatusBar *status)
{
    d->status = status;
    d->status->addWidget(d->navigation);
    d->status->addWidget(d->registration);
    d->status->addWidget(d->address);
    d->status->addWidget(d->search);

    d->navigation->show();
    d->registration->show();
    d->address->show();
    d->search->show();
}

void medDocumentationArea::setdw(QStatusBar *status)
{
    d->status = status;
    d->status->removeWidget(d->navigation);
    d->status->removeWidget(d->registration);
    d->status->removeWidget(d->address);
    d->status->removeWidget(d->search);

    d->navigation->hide();
    d->registration->hide();
    d->address->hide();
    d->search->hide();
}

void medDocumentationArea::onRegisterClicked(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Help Collection File", qApp->applicationDirPath(), "Help collection file (*.qch)");
    
    if(!fileName.isNull())
        dtkHelpController::instance()->registerDocumentation(fileName);
}

void medDocumentationArea::onUnregisterClicked(void)
{
    bool ok;

    QStringList items = dtkHelpController::instance()->registeredNamespaces();
    items.removeFirst();

    QString namespaceName = QInputDialog::getItem(
        this,
        "dtkAssistant",
        "Choose the documentation namespace to unregister",
        items,
        0,
        false,
        &ok);
    
    if (ok)
        dtkHelpController::instance()->unregisterDocumentation(namespaceName);
}

void medDocumentationArea::onUrlChanged(const QUrl& url)
{
    d->address->setText(url.toString());
}

void medDocumentationArea::onLinksActivated(const QMap<QString, QUrl>& urls, const QString& keyword)
{
    bool ok;
    
    QString key = QInputDialog::getItem(
        this,
        "dtkAssistant",
        QString("Choose a topic for %1").arg(keyword),
        urls.keys(),
        0,
        false,
        &ok);
    
    if (ok)
        d->browser->setSource(urls.value(key));
}

void medDocumentationArea::onAddressFocusTriggered(void)
{
    d->address->setFocus();
}

void medDocumentationArea::onSearchFocusTriggered(void)
{
    d->search->setFocus();
}
