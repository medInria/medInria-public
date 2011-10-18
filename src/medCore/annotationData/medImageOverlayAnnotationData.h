/*
 * msegmedImageOverlayAnnotationData.h
 *
 *  Created on: 31 mai 2011 10:36:54
 *      Author: jstark
 */

#ifndef MSEGmedImageOverlayAnnotationData_H_
#define MSEGmedImageOverlayAnnotationData_H_

#include "medCoreExport.h"

#include <medAnnotationData.h>

#include <QVector3D>

class MEDCORE_EXPORT medImageOverlayAnnotationData : public medAnnotationData {
    Q_OBJECT;
public:
    medImageOverlayAnnotationData();
    virtual ~medImageOverlayAnnotationData();

    QString description(void) const MED_OVERRIDE;
    QString identifier(void) const MED_OVERRIDE;

    /** Get name to use for this when registering with a factory.*/
    static QString s_identifier();

    static QString s_description();

protected:

};

}

#endif /* MSEGmedImageOverlayAnnotationData_H_ */

