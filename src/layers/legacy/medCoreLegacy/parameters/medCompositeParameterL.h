/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameterL.h>

#include <QVariant>

#include <medCoreLegacyExport.h>

class medCompositeParameterLPrivate;

class MEDCORELEGACY_EXPORT medCompositeParameterL : public medAbstractParameterL
{
    Q_OBJECT

public:
    medCompositeParameterL(QString name = "Unknown composite parameter", QObject* parent = 0);
    virtual ~medCompositeParameterL();

    virtual QWidget* getWidget();

    void addVariant(QString name, QVariant variant,
                    QVariant min = QVariant(0), QVariant max = QVariant(100), QVariant step = QVariant(1));

    virtual QList<QVariant> values() const;
    virtual QVariant value(const QString) const;
    QList<QPair <QVariant, QVariant> > ranges() const;
    QList<QVariant> steps() const;

    virtual void trigger();

public slots:
    virtual void setValues(const QHash<QString, QVariant> );

protected:
    virtual void updateInternWigets();

signals:
    void valuesChanged(const QHash<QString,QVariant>& );

private slots:
    void updateValue(bool value);
    void updateValue(int value);
    void updateValue(double value);

    void removeInternWidget(QObject *widget);

private:
    medCompositeParameterLPrivate* d;
};


