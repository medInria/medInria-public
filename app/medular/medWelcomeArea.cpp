#include "medWelcomeArea.h"

#include <QtCore>
#include <QtGui>
#include <QtWebKit>

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
    QWebView *web_view;
};

medWelcomeArea::medWelcomeArea(QWidget *parent) : QWidget(parent), d(new medWelcomeAreaPrivate)
{
    d->web_view = new QWebView(this);
    d->web_view->setContextMenuPolicy(Qt::NoContextMenu);
    d->web_view->setAcceptDrops(false);
    d->web_view->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
    d->web_view->settings()->setAttribute(QWebSettings::JavaEnabled, false);
    d->web_view->setHtml(readFile(QLatin1String(":/html/index.html")), QUrl("qrc:/html/index.html"));

    connect(d->web_view, SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->web_view);
}

medWelcomeArea::~medWelcomeArea(void)
{
    delete d;

    d = NULL;
}

void medWelcomeArea::linkClicked(const QUrl& url)
{
    Q_UNUSED(url);
}
