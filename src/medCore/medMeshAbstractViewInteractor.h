#ifndef MEDMESHABSTRACTVIEWINTERACTOR_H
#define MEDMESHABSTRACTVIEWINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "medCoreExport.h"

class MEDCORE_EXPORT medMeshAbstractViewInteractor: public dtkAbstractViewInteractor
{
    Q_OBJECT
public:
  medMeshAbstractViewInteractor(){};
  virtual ~medMeshAbstractViewInteractor(){};
  virtual char* getLUTQuery (int meshLayer)=0;
  virtual double opacity (int meshLayer)=0;
  virtual void setLayer(int meshLayer)=0;
  virtual int meshLayer(void)=0;
  virtual void setOpacity (double value) = 0;
  virtual bool visibility (int meshLayer) = 0;
  virtual bool edgeVisibility (int meshLayer) = 0;
  virtual QString* color (int meshLayer) = 0;
  virtual QString* renderingType (int meshLayer) = 0;
  virtual QString* attribute (int meshLayer) = 0;
  virtual QString* lut (int meshLayer) = 0;
  virtual void setAttribute (const QString& attribute, int meshLayer) = 0;
  virtual void setScalarVisibility(bool val) = 0;
  virtual bool isMeshOnly() = 0;
protected:

 signals:

public slots:
    virtual void onColorPropertySet (const QColor& color)=0;
   

private:

};

#endif // MED4DABSTRACTVIEWINTERACTOR_H
