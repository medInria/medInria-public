/*
 * msegImageOverlayAnnotationData.h
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#ifndef MSEGImageOverlayAnnotationData_H_
#define MSEGImageOverlayAnnotationData_H_

#include "msegPluginExport.h"

#include <medAnnotationData.h>

#include <QVector3D>

namespace mseg {

class ImageOverlayAnnotationData : public medAnnotationData {
    Q_OBJECT;
public:
    ImageOverlayAnnotationData();
    virtual ~ImageOverlayAnnotationData();

    QString description(void) const MED_OVERRIDE;
    QString identifier(void) const MED_OVERRIDE;

    /** Get name to use for this when registering with a factory.*/
    static QString s_identifier();

    static QString s_description();

protected:

};

}

#endif /* MSEGImageOverlayAnnotationData_H_ */

