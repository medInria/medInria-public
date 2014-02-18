/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameter.h>

#include <medGuiExport.h>

class QComboBox;
class QWidget;
class QStringList;

class medStringListParameterPrivate;
class MEDGUI_EXPORT medStringListParameter: public medAbstractStringParameter
{
    Q_OBJECT

public:
    medStringListParameter(QString name = "Unkow string list parameter", QObject* parent = 0);
    virtual ~medStringListParameter();

    void addItem(QString& item);
    void addItems(const QStringList& items);
    void removeItem(QString& item);
    void clear();

    QStringList& items() const;
    QComboBox* getComboBox();

    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

private slots:
    void removeInternComboBox();

private:
    medStringListParameterPrivate* d;
};
