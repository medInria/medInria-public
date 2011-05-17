#ifndef _itk_DataImageBase_h_
#define _itk_DataImageBase_h_

#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>

#include "itkDataImagePluginExport.h"

class itkDataImageBaseImpl;

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageBase : public dtkAbstractDataImage
{
    Q_OBJECT;

public:
    ~itkDataImageBase(void);
    virtual QString description(void) const;

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

protected:
    itkDataImageBase(const QString & description, itkDataImageBaseImpl * impl );

private:

    itkDataImageBaseImpl * d;
    QString m_description;

};

#endif /* _itk_DataImageBase_h_ */
