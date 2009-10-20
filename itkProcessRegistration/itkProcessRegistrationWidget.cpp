/* itkProcessRegistrationWidget.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jul 27 21:02:42 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  8 16:49:28 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "itkProcessRegistrationWidget.h"

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationWidgetPrivate
// /////////////////////////////////////////////////////////////////

class itkProcessRegistrationWidgetPrivate
{
public:
    QLineEdit *edit1;
    QLineEdit *edit2;

    QPushButton *choose_button_1;
    QPushButton *choose_button_2;
    QPushButton *run_button;
    QComboBox   *choose_reg_method;
};

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationWidget
// /////////////////////////////////////////////////////////////////

itkProcessRegistrationWidget::itkProcessRegistrationWidget(QWidget *parent) : QWidget(parent), d(new itkProcessRegistrationWidgetPrivate)
{
    QFormLayout *layout = new QFormLayout(this);
    
    d->edit1 = new QLineEdit(this);
    d->edit2 = new QLineEdit(this);
    d->choose_button_1 = new QPushButton("Choose fixed image", this);
    d->choose_button_2 = new QPushButton("Choose moving image", this);
    d->run_button = new QPushButton("Register", this);
    d->choose_reg_method = new QComboBox(this);
    d->choose_reg_method->addItems(QStringList("Rigid") << "Affine");
    
    layout->addRow(d->edit1, d->choose_button_1);
    layout->addRow(d->edit2, d->choose_button_2);
    layout->addWidget(d->run_button);
    layout->addWidget(d->choose_reg_method);
    
    connect(d->choose_button_1, SIGNAL(clicked()), this, SIGNAL(chooseFixedImage()));
    connect(d->choose_button_2, SIGNAL(clicked()), this, SIGNAL(chooseMovingImage()));
    connect(d->run_button, SIGNAL(clicked()), this, SIGNAL(run()));
    connect(d->choose_reg_method, SIGNAL(activated(QString)),this,SIGNAL(chooseRegMethod(QString)));
}

void itkProcessRegistrationWidget::setFixedImageLabel(QString label)
{
    d->edit1->setText(label);
}

void itkProcessRegistrationWidget::setMovingImageLabel(QString label)
{
    d->edit2->setText(label);
}
