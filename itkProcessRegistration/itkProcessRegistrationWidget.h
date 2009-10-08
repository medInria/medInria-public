/* itkProcessRegistrationWidget.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jul 27 21:00:03 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Jul 27 21:08:55 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
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

#include <dtkGui/dtkUi.h>

class itkProcessRegistrationWidgetPrivate;

class itkProcessRegistrationWidget : public dtkUi
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
