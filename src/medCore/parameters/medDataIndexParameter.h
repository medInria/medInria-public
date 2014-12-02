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
#include <QIcon>

#include <medCoreExport.h>

class QComboBox;
class QWidget;
class QStringList;
class medDropSite;

class medDataIndex;

class medDataListParameterPrivate;
class MEDCORE_EXPORT medDataIndexParameter: public medAbstractParameter
{
    Q_OBJECT

public:
    medDataIndexParameter(QString name = "Unknown data index parameter", QObject* parent = 0);
    virtual ~medDataIndexParameter();

    void setValue(medDataIndex values);
    void clear();
    medDataIndex value() const;

    virtual QWidget* getWidget();
    medDropSite* getDropSite();

public:
    void updateInternWigets();

public:
    void setText(const QString &text);

private:
    void removeInternDropSite();

signals:
    void valueChanged (medDataIndex);

private:
    medDataListParameterPrivate* d;
};
