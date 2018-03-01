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
#include <QIcon>

#include <medCoreLegacyExport.h>

class QComboBox;
class QWidget;
class QStringList;

class medStringListParameterLPrivate;
class MEDCORELEGACY_EXPORT medStringListParameterL: public medAbstractStringParameterL
{
    Q_OBJECT

public:
    static QIcon createIconFromColor(const QString &colorName);


    medStringListParameterL(QString name = "Unknown string list parameter", QObject* parent = 0);
    virtual ~medStringListParameterL();

    void addItem(QString item, QIcon icon = QIcon());
    void addItems(QStringList items);
    void removeItem(QString item);
    void clear();

    QStringList items() const;
    QComboBox* getComboBox();

    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

private slots:
    void removeInternComboBox();

private:
    medStringListParameterLPrivate* d;
};
