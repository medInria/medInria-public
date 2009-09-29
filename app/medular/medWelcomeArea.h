#ifndef MEDWELCOMEAREA_H
#define MEDWELCOMEAREA_H

#include <QtCore>
#include <QtGui>

class medWelcomeAreaPrivate;

class medWelcomeArea : public QWidget
{
    Q_OBJECT

public:
     medWelcomeArea(QWidget *parent = 0);
    ~medWelcomeArea(void);

protected slots:
    void linkClicked(const QUrl &url);

private:
    medWelcomeAreaPrivate *d;
};

#endif // MEDWELCOMEAREA_H
