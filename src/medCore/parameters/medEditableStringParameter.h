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

#include <medCoreExport.h>

class QWidget;

class medEditableStringParameterPrivate;
class MEDCORE_EXPORT medEditableStringParameter: public medAbstractStringParameter
{
    Q_OBJECT

public:
    medEditableStringParameter(QString name = "Unknow string parameter", QObject* parent = 0);
    virtual ~medEditableStringParameter();
    void clear();

    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

private slots:
    void removeInternWidget();

private:
    medEditableStringParameterPrivate* d;
};
