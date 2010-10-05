#ifndef _itk_DataImageUInt3_h_
#define _itk_DataImageUInt3_h_

#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>

#include "itkDataImagePluginExport.h"


class itkDataImageUInt3Private;

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageUInt3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageUInt3(void);
    ~itkDataImageUInt3(void);
    virtual QString description(void) const;
    static bool registered(void);

    virtual QImage        &thumbnail(void)   const;
    virtual QList<QImage> &thumbnails (void) const;

 public slots:
    // derived from dtkAbstractData
    
    void *output(void);
    void *data(void);
    void setData(void* data);
    void update(void);

    void onMetaDataSet(QString key, QString value);
    void onPropertySet(QString key, QString value);

 public:
    // derived from dtkAbstractDataImage

    int xDimension(void);
    int yDimension(void);
    int zDimension(void);
	int tDimension(void);

    int minRangeValue(void);
    int maxRangeValue(void);

    int scalarValueCount(int value);
    int scalarValueMinCount(void);
    int scalarValueMaxCount(void);
    
 private:

    itkDataImageUInt3Private* d;
  
};

dtkAbstractData* createItkDataImageUInt3 (void);

#endif
