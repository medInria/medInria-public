#include "medStatusBar.h"

#include <QtCore>

class medStatusBarPrivate
{
public:
    QBoxLayout * statusBarLayout;
};

medStatusBar::medStatusBar ( QWidget* parent ) : QStatusBar ( parent ), d ( new medStatusBarPrivate )
{
    d->statusBarLayout = NULL;
    this->statusBarWidth = this->size().width();
    this->availableSpace = this->statusBarWidth-175-16-21; // width of statusbar - workspaceButton - Quit - Fullscreen
}

medStatusBar::~medStatusBar()
{
    delete d;
    d= NULL;
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
    if (statusBarWidth!=this->size().width()){ // if window dimensions have changed
        availableSpace += (this->size().width()-statusBarWidth);    //update available space 
        statusBarWidth = this->size().width();                      // and statusbarWidth
    }   
        
    if ( widget ){
        if ( d->statusBarLayout ){
            if ( this->availableSpace > widget->size().width()){      // if enough space
                d->statusBarLayout->insertWidget ( 1, widget );
                this->availableSpace -= widget->size().width();       //update available space
            }
            else{
                this->widgetList.append(widget);    //put the widget into a list                  
            }
        }
    }
}

void medStatusBar::removeMessage ( QWidget* widget )
{
    widget->deleteLater();
    this->availableSpace += widget->size().width();       //update available space
    if(!this->widgetList.isEmpty()){                // if message waiting to be displayed
        addMessage(this->widgetList.takeFirst());
    }
}
