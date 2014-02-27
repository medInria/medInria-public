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
#include <QStringList>

class QComboBox;
class QWidget;

class QColor;
class QIcon;

class medColorListParameterPrivate;
class MEDGUI_EXPORT medColorListParameter: public medAbstractStringParameter
{
    Q_OBJECT

public:
    medColorListParameter(QString name = "Unkow color list parameter", QObject* parent = 0);
    virtual ~medColorListParameter();

    void addColor(const QString& colorName, const QString& textDisplayed = 0);
    void addColors(const QStringList& colorNames, const QStringList &labels = QStringList());
    void removeColor(const QString& colorName);
    void clear();
    void setCurrentColor(const QString& colorName);

    QList<QString> colors() const;
    QComboBox* getComboBox();

    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

protected:
    QIcon createIcon(const QString& colorName) const;

private slots:
    void removeInternComboBox();
    void setColor(const QString& label);

private:
    medColorListParameterPrivate* d;
};
