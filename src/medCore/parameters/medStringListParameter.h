/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameter.h>
#include <QIcon>

#include <medCoreExport.h>

class QComboBox;
class QWidget;
class QStringList;

class medStringListParameterPrivate;
class MEDCORE_EXPORT medStringListParameter: public medAbstractParameter
{
    Q_OBJECT

public:

    static QIcon createIconFromColor(const QString &colorName);

    medStringListParameter(QString name = "Unkow string list parameter", QObject* parent = 0);
    virtual ~medStringListParameter();

    virtual QString value() const;
    virtual QStringList values() const;

    void addItem(QString item, QIcon icon = QIcon());
    void addItems(QStringList items);
    void removeItem(QString item);
    void clear();

    QStringList items() const;
    QComboBox* getComboBox();

    virtual QWidget* getWidget();

    void setExclusive(bool exclusive);

public slots:
    virtual void setValue(QString value);
    virtual void setValues(QStringList value);

signals:
    void valueChanged(QString);
    void valueChanged(QStringList);

protected:
    virtual void updateInternWigets();

private slots:
    void removeInternComboBox();

private:
    medStringListParameterPrivate* d;
};
