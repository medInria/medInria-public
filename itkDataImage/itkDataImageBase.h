#ifndef _itk_DataImageBase_h_
#define _itk_DataImageBase_h_

#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>

#include "itkDataImagePluginExport.h"

class itkDataImageBaseImpl;
/**
 * This class serves as a common base for all itkDataImage classes.
 *
 * It hides the template implementations from Qt.
 *
 * The itkDataImageBaseImpl member (d) provides an abstract interface to templated functions.
 * This class is not designed to be instantiated directly, it should only be created
 * by derived classes (hence the protected constructor).
 * */
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
    /** Protected constructor : see note above */
    itkDataImageBase(const QString & description, itkDataImageBaseImpl * impl );

private:

    /** The d pointer is provided by derived classes. */
    itkDataImageBaseImpl * d;
    /** The data description, returned by this->description.
     *  Provided by derived classes in the constructor. */
    QString m_description;

};

#endif /* _itk_DataImageBase_h_ */
