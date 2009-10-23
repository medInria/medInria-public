#ifndef MEDADMINAREA_H
#define MEDADMINAREA_H

#include <QtGui/QWidget>

class medAdminAreaPrivate;

class medAdminArea : public QWidget
{
    Q_OBJECT

public:
     medAdminArea(QWidget *parent = 0);
    ~medAdminArea(void);

signals:
    void accepted(void);
    void rejected(void);

protected:
    void changeEvent(QEvent *e);

private:
    medAdminAreaPrivate *d;
};

#endif // MEDADMINAREA_H
