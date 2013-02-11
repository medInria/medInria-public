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
    this->availableSpace = -1; //value before initialization (after, always positive)
    message_count = 0;
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

void medStatusBar::setAvailableSpace ( int space )
{
    this->statusBarWidth = this->size().width();
    this->availableSpace = space;
}

int medStatusBar::getAvailableSpace ( void )
{
    return this->availableSpace;
}

void medStatusBar::setMessageCount(int number)
{
    this->message_count = number;
}

int medStatusBar::getMessageCounter()
{
    return this->message_count;
}


/**
 * Initialize availableSpace
*/
void medStatusBar::init_availableSpace()
{
    if (availableSpace == -1 )
    {
        qDebug()<<" INITIALIZATION";
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
        qDebug()<<" WINDOW SIZE CHANGED";
        qDebug()<<"       previous space = "<<availableSpace;
        availableSpace += (this->size().width()-statusBarWidth);    //update available space 
        qDebug()<<"       new space = "<<availableSpace;
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
                message_count ++;
                message->startTimer(); // starts the countdown before removing the message
                d->statusBarLayout->insertWidget ( 1, message );
            }
            else
            {
                this->hiddenMessageList.append(message);    //put the message into a list 
                qDebug()<<"hiddenMessageList.count() addMessage : "<<hiddenMessageList.count();
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
        this->messageList.takeFirst()->deleteLater();
        this->availableSpace += (message->size().width()+d->statusBarLayout->spacing());         //update available space

        d->statusBarLayout->removeWidget(message);
        
        message_count --;

        showHiddenMessage();    //space has been freed
    }
}

/**
 * If needed (= not enough space on the statusBar), hide the last notification when the quitMessage is displayed
*/
void medStatusBar::hideMessage( )
{
    qDebug()<<"   Hide Message !!";
    medMessage* messageToHide = this->messageList.takeFirst(); //take the last notification
    messageToHide->stopTimer();       //stop the timer for not being deleted while hidden
    messageToHide->hide();
    this->availableSpace += messageToHide->width() + d->statusBarLayout->spacing(); //update available space
    this->hiddenMessageList.append(messageToHide);
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
 * Updates the available space on the statusBar, when window size is modified.
*/
void medStatusBar::resizeEvent ( QResizeEvent* event )
{
    if(availableSpace != -1) // not before initialization
    spaceManagement();
}
