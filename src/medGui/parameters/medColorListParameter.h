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
class QColor;
class QIcon;

class medColorListParameterPrivate;
class MEDGUI_EXPORT medColorListParameter: public medAbstractStringParameter
{
    Q_OBJECT

public:
    medColorListParameter(QString name = "Unkow color list parameter", QObject* parent = 0);
    virtual ~medColorListParameter();

    void addColor(QString& colorName);
    void addColors(QStringList& colorNames);
    void removeColor(QString& colorNames);
    void clear();

    QStringList& colors() const;
    QComboBox* getComboBox();

    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

protected:
    QIcon createIcon(QString& colorName) const;

private slots:
    void _prvt_removeInternComboBox();

private:
    medColorListParameterPrivate* d;
};
