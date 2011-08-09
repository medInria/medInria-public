#include "medStatusBar.h"

class medStatusBarPrivate
{
public:
    QBoxLayout * statusBarLayout;
};

medStatusBar::medStatusBar ( QWidget* parent ) : QStatusBar ( parent ), d ( new medStatusBarPrivate )
{
    d->statusBarLayout = NULL;
}

void medStatusBar::setStatusBarLayout ( QBoxLayout* layout )
{
    d->statusBarLayout = layout;
}

QBoxLayout* medStatusBar::statusBarLayout()
{
    return d->statusBarLayout;
}

void medStatusBar::addMessage ( QWidget* widget )
{
    if ( d->statusBarLayout )
        d->statusBarLayout->insertWidget ( 1, widget );
}

void medStatusBar::removeMessage ( QWidget* widget )
{
    if ( d->statusBarLayout )
        d->statusBarLayout->removeWidget ( widget );
}