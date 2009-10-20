/* itkProcessRegistrationWidget.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jul 27 21:00:03 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  8 16:49:10 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 11
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef ITKPROCESSREGISTRATIONWIDGET_H
#define ITKPROCESSREGISTRATIONWIDGET_H

#include <QtCore>
#include <QtGui>

class itkProcessRegistrationWidgetPrivate;

class itkProcessRegistrationWidget : public QWidget
{
    Q_OBJECT

public:
    itkProcessRegistrationWidget(QWidget *parent = 0);

public slots:
    void setFixedImageLabel(QString label);
    void setMovingImageLabel(QString label);

signals:
    void run(void);
    void chooseFixedImage(void);
    void chooseMovingImage(void); 
    void chooseRegMethod(QString);
 
private:
    itkProcessRegistrationWidgetPrivate *d;
};

#endif // ITKPROCESSREGISTRATIONWIDGET_H
