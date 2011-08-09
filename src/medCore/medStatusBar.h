#ifndef MEDSTATUSBAR_H
#define MEDSTATUSBAR_H

#include <QStatusBar>
#include <QWidget>
#include <QBoxLayout>


class medStatusBarPrivate;

class medStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    medStatusBar ( QWidget * parent = 0 );

    void setStatusBarLayout ( QBoxLayout * layout );

    QBoxLayout * statusBarLayout();

    void addMessage(QWidget * widget);

    void removeMessage(QWidget * widget);

private:
    medStatusBarPrivate * d;
};

#endif // MEDSTATUSBAR_H
