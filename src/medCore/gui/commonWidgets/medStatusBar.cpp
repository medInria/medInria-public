/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStatusBar.h>

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
    this->availableSpace = -1; //value before initialization (after, always positive)
}

medStatusBar::~medStatusBar()
{
    delete d;
    d= NULL;
}

/**
 *  Set the status bar layout containing the widgets
 * (messages, quick access menu, quit button)
 *
 * @param : QBoxLayout * layout
 */
void medStatusBar::setStatusBarLayout ( QBoxLayout* layout )
{
    d->statusBarLayout = layout;
}

/**
 *  Get the status bar layout
 *
 * @return : QBoxLayout *
 */
QBoxLayout* medStatusBar::statusBarLayout()
{
    return d->statusBarLayout;
}

/**
 *  Set the available space on the statusBar
 *
 * @param : int space
 */
void medStatusBar::setAvailableSpace ( int space )
{
    this->statusBarWidth = this->size().width();
    this->availableSpace = space;
}

int medStatusBar::getAvailableSpace()
{
    return this->availableSpace;
}


/**
 * Initialize availableSpace
*/
void medStatusBar::init_availableSpace()
{
    if (availableSpace == -1 )
    {
        emit initializeAvailableSpace();
    } 
}

/**
 * Update availableSpace when window is resized
*/
void medStatusBar::spaceManagement()
{
    if (availableSpace != -1)       // if availableSpace initialized
    {
        availableSpace += (this->size().width()-statusBarWidth);    //update available space 
        statusBarWidth = this->size().width();                      // and statusbarWidth
    } 
}

/**
 * Add a message to the statusBar if there's enough space, otherwise put it in a queue
 *
 * @param : medMessage * message
*/
void medStatusBar::addMessage ( medMessage* message )
{
    this->init_availableSpace();

    if ( message )
    {
        if ( d->statusBarLayout )
        {
            if ( this->availableSpace > message->size().width()+d->statusBarLayout->spacing())      // if enough space
            {      
                this->messageList.append(message);   // messages are stored in a list to easily hide newest messages 
                                                     // when quit message is displayed
                this->availableSpace -= (message->size().width() +d->statusBarLayout->spacing());       //update available space
                message->startTimer(); // starts the countdown before removing the message
                d->statusBarLayout->insertWidget ( 1, message );
            }
            else
            {
                this->hiddenMessageList.append(message);    //put the message into a list 
                
            }
        }
    }
}

/**
 * Remove a message from the statusBar 
 * Slot called when a medMessage QTimer sends timeout()
 *
 * @param : medMessage * message
*/
void medStatusBar::removeMessage ( medMessage* message )
{
    if ( message )
    {
        this->messageList.removeFirst();
        this->availableSpace += (message->size().width()+d->statusBarLayout->spacing());         //update available space
        d->statusBarLayout->removeWidget(message);
        showHiddenMessage();    //space has been freed
    }
}

/**
 * If needed (= not enough space on the statusBar), hide the last notification when the quitMessage is displayed
*/
void medStatusBar::hideMessage( )
{
    if ( !messageList.isEmpty() )
    {
        medMessage* messageToHide = this->messageList.takeFirst(); //take the last notification
        messageToHide->stopTimer();       //stop the timer for not being deleted while hidden
        messageToHide->hide();
        this->availableSpace += messageToHide->width() + d->statusBarLayout->spacing(); //update available space
        this->hiddenMessageList.append(messageToHide);
    }
}

/**
 * When space on the statusBar has been freed, show hidden messages
*/
void medStatusBar::showHiddenMessage()
{
    if(!this->hiddenMessageList.isEmpty())                         // if message waiting to be displayed
    {
        medMessage* messageToShow = hiddenMessageList.takeFirst();
        addMessage(messageToShow);
    }
}

/**
 * Update the available space on the statusBar, when window size is modified.
*/
void medStatusBar::resizeEvent ( QResizeEvent* event )
{
    if(availableSpace != -1) // not before initialization
    {
        spaceManagement();

        int diff = event->size().width() - event->oldSize().width();

        if ( diff > 0 )
        {
            showHiddenMessage();
        }
        else
        {
            if(qAbs(diff) > availableSpace)
            {
                hideMessage();
            }
        }
    }
}
