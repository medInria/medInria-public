#ifndef MEDSTATUSBAR_H
#define MEDSTATUSBAR_H

#include <QStatusBar>
#include <QWidget>
#include <QBoxLayout>


class medStatusBarPrivate;

/**
* @class  medStatusBar
* @author Alexandre Abadie
* @brief  This is extended QStatusBar that can display custom messages.
*/

class medStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    /**
     *  Constructor
     */
    medStatusBar ( QWidget * parent = 0 );

    /**
     *  Set the status bar layout containing the widgets
     * (messages, quick access menu, quit button)
     *
     * @param : QBoxLayout * layout
     */
    void setStatusBarLayout ( QBoxLayout * layout );

    /**
     *  Set the status bar layout
     *
     * @return : QBoxLayout *
     */
    QBoxLayout * statusBarLayout();

public slots:
    /**
     * Add a widget containing a message (information, progress, error)
     * just after the quick access menu
     *
     * @param : QWidget * widget
     */
    void addMessage ( QWidget * widget );

    /**
     *  Remove a widget (containing a message)
     */
    void removeMessage ( QWidget * widget );

private:
    medStatusBarPrivate * d;
};

#endif // MEDSTATUSBAR_H
