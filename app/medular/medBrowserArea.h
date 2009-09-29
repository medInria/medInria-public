#ifndef MEDBROWSERAREA_H
#define MEDBROWSERAREA_H

#include <QWidget>

class medBrowserAreaPrivate;

class medBrowserArea : public QWidget
{
public:
     medBrowserArea(QWidget *parent = 0);
    ~medBrowserArea(void);

private:
    medBrowserAreaPrivate *d;
};

#endif // MEDBROWSERAREA_H
