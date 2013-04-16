/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "medCoreExport.h"

class MEDCORE_EXPORT medMeshAbstractViewInteractor: public dtkAbstractViewInteractor
{
    Q_OBJECT
public:
  medMeshAbstractViewInteractor(){}
  virtual ~medMeshAbstractViewInteractor(){}

  virtual void setOpacity(int meshLayer, double value) = 0;
  virtual double opacity(int meshLayer) const = 0;

  virtual void setVisibility(int meshLayer, bool visible) = 0;
  virtual bool visibility(int meshLayer) const = 0;

  virtual void setEdgeVisibility(int meshLayer, bool visible) = 0;
  virtual bool edgeVisibility(int meshLayer) const = 0;

  virtual void setColor(int meshLayer, QColor color) = 0;
  virtual QColor color(int meshLayer) const = 0;

  virtual void setRenderingType(int meshLayer, const QString & type) = 0;
  virtual QString renderingType(int meshLayer) const = 0;

  virtual void setAttribute(int meshLayer, const QString & attribute) = 0;
  virtual QString attribute(int meshLayer) const = 0;

  virtual QStringList getAllAttributes(int meshLayer) const = 0;

  virtual void setLut(int meshLayer, const QString & lut) = 0;
  virtual QString lut(int meshLayer) const = 0;

  virtual QStringList getAllLUTs() const = 0;
};


