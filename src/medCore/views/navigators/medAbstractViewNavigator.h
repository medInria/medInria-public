/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractExtraNavigator.h>

#include <medCoreExport.h>

class MEDCORE_EXPORT medAbstractViewNavigator : public medAbstractExtraNavigator
{
    Q_OBJECT

public:
    medAbstractViewNavigator(medAbstractView* parent);
virtual ~medAbstractViewNavigator();

    virtual double zoom() const = 0;
    virtual QVector2D pan() const = 0;

    /**
    * @brief implementationOf
    * @return Upper abstract class it derives from.
    * Do NOT reimplement it in non abstract class.
    * Used by the factory to kwnow what can be create.
    */
    static QString implementationOf()
    {
        return "medAbstractViewNavigator";
    }
public slots:
     virtual void setZoom (double zoom) =0;
     virtual void setPan (const QVector2D &pan) = 0;
};
