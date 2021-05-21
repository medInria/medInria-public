#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameterPresenter.h>
#include <medVariantListParameter.h>
#include <QWidget>
#include <QFormLayout>

//class QCheckBox;
//class QRadioButton;
//class QPushButton;
//class QAbstractButton;
//class medCompositeParameterPresenterPrivate;


class medVariantListParameterWidgetPrivate : public QWidget
{
    Q_OBJECT
public:
    medVariantListParameterWidgetPrivate(QWidget *parent = nullptr, const char *name = nullptr);
    ~medVariantListParameterWidgetPrivate();

public slots:

    void changeValue(QString const &key, QVariant const &Value);
    void addVariant(QString const &key, QVariant const &Value);
    void removeVariant(QString const &key);


signals:
    void valueChanged(QString const &key, QVariant const &Value);

private:
    void blockInternWidgetsSignals(bool block) const;

    QFormLayout layout;
    QHash<QString, QWidget*> widgets;
};
