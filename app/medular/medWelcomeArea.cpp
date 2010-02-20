/* medWelcomeArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct  5 08:29:35 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 23:39:28 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 156
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medWelcomeArea.h"

#include <QtWebKit>

#include <medGui/medLoginWidget.h>

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

class medWelcomeAreaPrivate
{
public:
//    QWebView *web_view;

    medLoginWidget *login;
};

medWelcomeArea::medWelcomeArea(QWidget *parent) : QWidget(parent), d(new medWelcomeAreaPrivate)
{
    // d->web_view = new QWebView(this);
    // d->web_view->setContextMenuPolicy(Qt::NoContextMenu);
    // d->web_view->setAcceptDrops(false);
    // d->web_view->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    // d->web_view->settings()->setAttribute(QWebSettings::JavaEnabled, true);
    // // d->web_view->setHtml(readFile(QLatin1String(":/html/index.html")), QUrl("qrc:/html/index.html"));
    // d->web_view->setUrl(QUrl("http://www.youtube.com/html5"));

    // // qDebug() << d->web_view->settings()->pluginDatabase()->searchPaths();

    // connect(d->web_view, SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl)));

    d->login = new medLoginWidget(this);

    connect(d->login, SIGNAL(accepted()), this, SIGNAL(accepted()));
    connect(d->login, SIGNAL(rejected()), this, SIGNAL(rejected()));

//    QVBoxLayout *layout = new QVBoxLayout(this);
//    layout->setContentsMargins(0, 0, 0, 0);
//    layout->setSpacing(0);
//    // layout->addWidget(d->web_view);
//    layout->addStretch(8);
//    layout->addWidget(d->login);
//    layout->addStretch(1);
}

medWelcomeArea::~medWelcomeArea(void)
{
    delete d;

    d = NULL;
}

void medWelcomeArea::setup(QStatusBar *status)
{
    status->addWidget(d->login, 1);

    d->login->show();
}

void medWelcomeArea::setdw(QStatusBar *status)
{
    status->removeWidget(d->login);

    d->login->hide();
}

void medWelcomeArea::paintEvent(QPaintEvent *event)
{
    QPixmap pixmap(":/pixmaps/medular-logo.png");

    QPainter painter;
    painter.begin(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.fillRect(event->rect(), QColor(0x31, 0x31, 0x31));
    painter.drawPixmap(
        event->rect().width()/2-pixmap.width()/2,
        event->rect().height()/2-pixmap.height()/2, pixmap);
    painter.end();

    QWidget::paintEvent(event);
}

void medWelcomeArea::linkClicked(const QUrl& url)
{
    Q_UNUSED(url);
}
