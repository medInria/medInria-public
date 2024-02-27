#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStringParameter.h>

#include <QIcon>
#include <QValidator>

class QWidget;
class QStringList;

class medStringListParameterPrivate;
class MEDCORE_EXPORT medStringListParameter: public medStringParameter
{
    Q_OBJECT

public:

    medStringListParameter(QString name = "Unknown string list parameter", QObject* parent = nullptr);
    ~medStringListParameter() override;

    medParameterType type() const override { return medParameterType::MED_PARAMETER_STRING_LIST; }

    void addItem(QString item, QIcon icon = QIcon());
    void addItems(QStringList items);
    void removeItem(QString item);
    void clear();

    QStringList items() const;
    QString value() const override;
    int getIndex() const;

    bool copyValueTo(medAbstractParameter &dest) override;

    void setValidator(QValidator *pi_poValidator) override;
    QValidator *getValidator() const override;

    

public slots:
    bool setValue(QString const& value) override;
    bool setValue(QString const& value, int index);
    bool setIndex(int index);
    void trigger() override;

signals:
    void valueChanged(int const& value);
    void compositionChanged();

private:
    void reComputeIndex(QString const& pi_roCurentValue);

    const QScopedPointer<medStringListParameterPrivate> d;
};

static QIcon createIconFromColor(const QString &colorName); //TODO Check if it used ?