#ifndef _itk_DataImageUShort3_h_
#define _itk_DataImageUShort3_h_

#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>

#include "itkDataImagePluginExport.h"

class itkDataImageUShort3Private;

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageUShort3 : public dtkAbstractDataImage
{
    Q_OBJECT
    
public:
    itkDataImageUShort3(void);
    ~itkDataImageUShort3(void);
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

    void onMetaDataSet(const QString& key, const QString& value);
    void onPropertySet(const QString& key, const QString& value);

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
    void computeRange(void);
    void computeValueCounts(void);

    itkDataImageUShort3Private* d;

};

dtkAbstractData* createItkDataImageUShort3 (void);

#endif
