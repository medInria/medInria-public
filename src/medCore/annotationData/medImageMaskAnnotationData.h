/*
 * msegSeedPointAnnotationData.h
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#ifndef MSEGSEEDPOINTANNOTATIONDATA_H_
#define MSEGSEEDPOINTANNOTATIONDATA_H_

#include "medCoreExport.h"

#include <medAnnotationData.h>
#include <medAbstractDataImage.h>

#include <dtkCore/dtkSmartPointer.h>

#include <QtCore>

class MEDCORE_EXPORT medImageMaskAnnotationData : public medAnnotationData {
    Q_OBJECT;
public:
    medImageMaskAnnotationData();
    virtual ~medImageMaskAnnotationData();

    QString description(void) const;
    QString identifier(void) const;

    typedef QList< QPair<double, QColor> > ColorMapType;
    const ColorMapType & colorMap();
    void setColorMap(const ColorMapType & colorMap);

    // Implement dtkAbstractData
    virtual void *data(void);
    virtual void setData(void* data);

    // static methods
    static QString s_description();
    static QString s_identifier();

    medAbstractDataImage * maskData();
    void setMaskData( medAbstractDataImage * data );

protected:
    ColorMapType m_colorMap;
    dtkSmartPointer<medAbstractDataImage> m_maskData;
};

#endif /* MSEGSEEDPOINTANNOTATIONDATA_H_ */

