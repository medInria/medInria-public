/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStatusBar.h>

#include <QtCore>
#include <mutex>

class medStatusBarPrivate
{
public:
    QBoxLayout * statusBarLayout;
    int availableSpace; //available space on the status bar
    int statusBarWidth;
    std::recursive_mutex acccessMutex;      // mutex to guard conflict between external signals and/or internal update
    QList<medMessage *> hiddenMessageList; // storing messages waiting to be displayed
    QList<medMessage *> messageList;       // storing displayed messages
};

medStatusBar::medStatusBar ( QWidget* parent ) : QStatusBar ( parent ), d ( new medStatusBarPrivate )
{
    d->statusBarLayout = nullptr;
    d->statusBarWidth = this->size().width();
    d->availableSpace = -1; //value before initialization (after, always positive)
}

medStatusBar::~medStatusBar()
{
    delete d;
    d = nullptr;
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
    d->acccessMutex.lock();    
    d->statusBarWidth = this->size().width();
    d->availableSpace = space;
    d->acccessMutex.unlock();
}

int medStatusBar::getAvailableSpace()
{
    d->acccessMutex.lock();
    int iRes = d->availableSpace;
    d->acccessMutex.unlock();
    return iRes;
}


/**
 * Initialize availableSpace
*/
void medStatusBar::init_availableSpace()
{
    if (getAvailableSpace() == -1 ) // no need to lock mutex because is done by getAvailableSpace()
    {
        emit initializeAvailableSpace();
    } 
}

/**
 * Update availableSpace when window is resized
*/
void medStatusBar::spaceManagement()
{
    d->acccessMutex.lock();
    if (d->availableSpace != -1)       // if availableSpace initialized
    {
        d->availableSpace += (this->size().width()- d->statusBarWidth);    //update available space 
        d->statusBarWidth = this->size().width();                      // and statusbarWidth
    }
    d->acccessMutex.unlock();
}

/**
 * Add a message to the statusBar if there's enough space, otherwise put it in a queue
 *
 * @param : medMessage * message
*/
void medStatusBar::addMessage ( medMessage* message )
{
    d->acccessMutex.lock();
    this->init_availableSpace();
    if ( message )
    {
        if ( d->statusBarLayout )
        {
            if ( d->availableSpace > message->size().width()+d->statusBarLayout->spacing())      // if enough space
            {
                d->messageList.append(message);   // messages are stored in a list to easily hide newest messages 
                                                  // when quit message is displayed
                d->availableSpace -= (message->size().width() +d->statusBarLayout->spacing());       //update available space
                message->startTimer(); // starts the countdown before removing the message
                d->statusBarLayout->insertWidget ( 1, message );
            }
            else
            {
                d->hiddenMessageList.append(message);    //put the message into a list              
            } 
        }
    }
    d->acccessMutex.unlock(); 
}

/**
 * Remove a message from the statusBar 
 * Slot called when a medMessage QTimer sends timeout()
 *
 * @param : medMessage * message
*/
void medStatusBar::removeMessage ( medMessage* message )
{
    d->acccessMutex.lock();
    if ( message )
    {
        d->messageList.removeOne(message);
        d->availableSpace += (message->size().width()+d->statusBarLayout->spacing());         //update available space
        d->statusBarLayout->removeWidget(message);
        showHiddenMessage();    //space has been freed
    }
    d->acccessMutex.unlock();
}

/**
 * If needed (= not enough space on the statusBar), hide the last notification when the quitMessage is displayed
*/
void medStatusBar::hideMessage( )
{
    d->acccessMutex.lock();
    if ( !d->messageList.isEmpty() )
    {
        medMessage* messageToHide = d->messageList.takeFirst(); //take the last notification
        messageToHide->stopTimer();       //stop the timer for not being deleted while hidden
        messageToHide->hide();
        d->availableSpace += messageToHide->width() + d->statusBarLayout->spacing(); //update available space
        d->hiddenMessageList.append(messageToHide);
    }
    d->acccessMutex.unlock();
}

/**
 * When space on the statusBar has been freed, show hidden messages
*/
void medStatusBar::showHiddenMessage()
{
    d->acccessMutex.lock();
    if(!d->hiddenMessageList.isEmpty())                         // if message waiting to be displayed
    {
        medMessage* messageToShow = d->hiddenMessageList.takeFirst();
        addMessage(messageToShow);
    }
    d->acccessMutex.unlock();
}

/**
 * Update the available space on the statusBar, when window size is modified.
*/
void medStatusBar::resizeEvent ( QResizeEvent* event )
{
    d->acccessMutex.lock();
    if(d->availableSpace != -1) // not before initialization
    {
        spaceManagement();

        int diff = event->size().width() - event->oldSize().width();

        if ( diff > 0 )
        {
            showHiddenMessage();
        }
        else
        {
            if(qAbs(diff) > d->availableSpace)
            {
                hideMessage();
            }
        }
    }
    d->acccessMutex.unlock();
}
