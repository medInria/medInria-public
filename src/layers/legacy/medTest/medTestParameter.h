/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medTestExport.h>
#include <medAbstractParameterL.h>

#include <QWidget>

class medTestParameterPrivate;
/**
 * @brief The medTestParameter class is just a dummy implementaion of
 * medAbstractParameterL to test its interface.
 */
class MEDTEST_EXPORT medTestParameter : public medAbstractParameterL
{
    Q_OBJECT

public:
    medTestParameter(QString name = "unknown parameter");
    virtual~medTestParameter();

    /**
     * @brief getWidget Return a default widget.
     * @return QWidget*
     */
    virtual QWidget* getWidget();

private slots:
    void removeInternWidget();

private:
  medTestParameterPrivate *d;
};

