#ifndef MEDSPLASHSCREEN_H
#define MEDSPLASHSCREEN_H

#include <QtGui>

////////////////////////////////////////////////////////////////////////////
class medSplashScreen : public QFrame
{
    Q_OBJECT
public:
 medSplashScreen(const QPixmap& pixmap);
 void finish(QWidget *w);
 void repaint();

public slots:
 void clearMessage();
 void showMessage(const QString& theMessage, int theAlignment = Qt::AlignLeft, const QColor& theColor = Qt::black);

private:
 virtual void paintEvent(QPaintEvent* pe);

 QPixmap  itsPixmap;
 QString  itsMessage;
 int   itsAlignment;
 QColor  itsColor;
};
#endif // MEDSPLASHSCREEN_H
