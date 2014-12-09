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

class medDataIndexParameterPrivate;
class MEDCORE_EXPORT medDataIndexParameter: public medAbstractParameter
{
    Q_OBJECT

public:
    medDataIndexParameter(QString name = "Unknown data index parameter", QObject* parent = 0);
    virtual ~medDataIndexParameter();

public:
    medDataIndex value() const;

public slots:
    void setValue(medDataIndex values);
    void clear();

signals:
    void valueChanged (medDataIndex);

public:
    void setText(const QString &text);

public:
    virtual QWidget* getWidget();
    medDropSite* getDropSite();

protected:
    void updateInternWigets();

private slots:
    void removeInternDropSite();

private:
    medDataIndexParameterPrivate* d;
};
