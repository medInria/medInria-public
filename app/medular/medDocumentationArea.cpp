/* medDocumentationArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 22 09:04:25 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 22 09:04:48 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDocumentationArea.h"

#include <QtHelp>
#include <QtWebKit>

// /////////////////////////////////////////////////////////////////
// Helper functions
// /////////////////////////////////////////////////////////////////

static QString readFile(const QString &name)
{
    QFile f(name);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("Unable to open %s: %s", name.toUtf8().constData(), f.errorString().toUtf8().constData());
        return QString();
    }
    QTextStream ts(&f);
    return ts.readAll();
}

// /////////////////////////////////////////////////////////////////
// medDocumentationArea
// /////////////////////////////////////////////////////////////////

class medDocumentationAreaPrivate
{
public:
    QWebView *view;

    QWidget *navigation;
    QPushButton *previous_button;
    QPushButton *next_button;
    QPushButton *go_button;
    QLineEdit *address_edit;

    QAction *previous_action;
    QAction *next_action;

    QProgressBar *progress;
    QStatusBar *status;
};

medDocumentationArea::medDocumentationArea(QWidget *parent) : QWidget(parent), d(new medDocumentationAreaPrivate)
{
    d->view = new QWebView(this);
    d->view->setContextMenuPolicy(Qt::NoContextMenu);
    d->view->setAcceptDrops(false);
    d->view->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    d->view->settings()->setAttribute(QWebSettings::JavaEnabled, true);
    d->view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#ifdef Q_WS_MAC
    d->view->setUrl(QUrl(qApp->applicationDirPath() + "/../../../../doc/index.html"));
#else
    d->view->setUrl(QUrl(qApp->applicationDirPath() + "../doc/index.html"));
#endif

    d->previous_action = d->view->pageAction(QWebPage::Back);
    d->next_action = d->view->pageAction(QWebPage::Back);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->view);

    d->navigation = new QWidget(this);
    d->previous_button = new QPushButton(d->navigation);
    d->previous_button->setIcon(QIcon(":/pixmaps/documentation_previous_arrow.png"));
    d->previous_button->setObjectName("medDocumentationAreaPreviousButton");
    d->previous_button->setEnabled(false);
    d->next_button = new QPushButton(d->navigation);
    d->next_button->setIcon(QIcon(":/pixmaps/documentation_next_arrow.png"));
    d->next_button->setObjectName("medDocumentationAreaNextButton");
    d->next_button->setEnabled(false);
    d->go_button = new QPushButton(d->navigation);
    d->go_button->setIcon(QIcon(":/pixmaps/documentation_go_arrow.png"));
    d->go_button->setObjectName("medDocumentationAreaGoButton");
    d->address_edit = new QLineEdit(d->navigation);
    d->address_edit->setObjectName("medDocumentationAreaAddressEdit");

    connect(d->previous_button, SIGNAL(clicked()), d->previous_action, SLOT(trigger()));
    connect(d->next_button, SIGNAL(clicked()), d->next_action, SLOT(trigger()));

    QHBoxLayout *navigation_buttons_layout = new QHBoxLayout;
    navigation_buttons_layout->setContentsMargins(0, 0, 0, 0);
    navigation_buttons_layout->setSpacing(0);
    navigation_buttons_layout->addWidget(d->previous_button);
    navigation_buttons_layout->addWidget(d->next_button);

    QHBoxLayout *navigation_address_layout = new QHBoxLayout;
    navigation_address_layout->setContentsMargins(0, 0, 0, 0);
    navigation_address_layout->setSpacing(0);
    navigation_address_layout->addWidget(d->address_edit);
    navigation_address_layout->addWidget(d->go_button);

    QHBoxLayout *navigation_layout = new QHBoxLayout(d->navigation);
    navigation_layout->setContentsMargins(0, 0, 0, 0);
    navigation_layout->setSpacing(10);
    navigation_layout->addLayout(navigation_buttons_layout);
    navigation_layout->addLayout(navigation_address_layout);

    connect(d->address_edit, SIGNAL(returnPressed()), SLOT(load()));
    connect(d->go_button, SIGNAL(clicked()), SLOT(load()));

    connect(d->view, SIGNAL(loadProgress(int)), this, SLOT(onLoadProgress(int)));
    connect(d->view, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished()));

    d->progress = new QProgressBar;
    d->progress->setMinimum(0);
    d->progress->setMaximum(100);

    d->status = NULL;
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
    d->status->addWidget(d->progress);

    d->navigation->show();
    d->progress->show();
}

void medDocumentationArea::setdw(QStatusBar *status)
{
    d->status = status;
    d->status->removeWidget(d->navigation);
    d->status->removeWidget(d->progress);

    d->navigation->hide();
    d->progress->hide();
}

void medDocumentationArea::load(void)
{
    d->view->load(QUrl(d->address_edit->text()));
    d->view->setFocus();
}

void medDocumentationArea::onLoadProgress(int progress)
{
    d->progress->setValue(progress);
}

void medDocumentationArea::onLoadFinished(void)
{
    d->previous_button->setEnabled(d->previous_action->isEnabled());
    d->next_button->setEnabled(d->next_action->isEnabled());
    d->address_edit->setText(d->view->url().toString());
    d->progress->setValue(100);
}
