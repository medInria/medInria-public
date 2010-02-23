#ifndef _med_ITKProcessObjectObserver_h_
#define _med_ITKProcessObjectObserver_h_

#include <QObject>


/** Helper class that interface Observers with Qt Signals */
class QtSignalAdaptor : public QObject
{
  Q_OBJECT

public:
  QtSignalAdaptor()
    {
    }

  virtual ~QtSignalAdaptor() {}


  void EmitSignal(int value)
    {
    emit Signal(value);
    }
  
signals:
  void Signal(int);
  
};


#endif
