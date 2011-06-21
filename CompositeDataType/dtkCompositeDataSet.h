#ifndef _dtk_CompositeDataSet_h_
#define _dtk_CompositeDataSet_h_

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>

#include "CompositeDataTypePluginExport.h"

class dtkCompositeDataSetPrivate;



/**
    
    \class dtkCompositeDataSet
    
    \see dtkCompositeDataSet
    \author Nicolas Toussaint
*/

class COMPOSITEDATATYPEPLUGIN_EXPORT dtkCompositeDataSet : public dtkAbstractData
{
  Q_OBJECT
    
 public:
    dtkCompositeDataSet(void);
    ~dtkCompositeDataSet(void);
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
    // derived from dtkAbstractDataImage
    
 private:

    dtkCompositeDataSetPrivate* d;
  
};

dtkAbstractData* createVtkDataMesh (void);

#endif
