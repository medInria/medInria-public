#ifndef _med_CompositeDataSet_h_
#define _med_CompositeDataSet_h_

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>

#include "medCompositeDataTypesPluginExport.h"

class medCompositeDataSetPrivate;



/**
    
    \class medCompositeDataSet
    
    \see medCompositeDataSet
    \author Nicolas Toussaint
*/

class MEDCOMPOSITEDATATYPESPLUGIN_EXPORT medCompositeDataSet : public dtkAbstractData
{
  Q_OBJECT
    
 public:
    medCompositeDataSet(void);
    ~medCompositeDataSet(void);
    virtual QString description(void) const;
    static bool registered(void);

    virtual QImage        &thumbnail  (void) const;
    virtual QList<QImage> &thumbnails (void) const;

 public slots:
    // derived from dtkAbstractData
    
    void *output(void);
    void *data(void);
    void setData(void* data);
    void update(void);
    
 public:
    
 private:

    medCompositeDataSetPrivate* d;
  
};

dtkAbstractData* createMedCompositeDataSet (void);

#endif
