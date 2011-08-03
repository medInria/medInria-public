#ifndef MSEGSEEDPOINTANNOTATION_H
#define MSEGSEEDPOINTANNOTATION_H

#include "msegPluginExport.h"

#include "medAnnotationGraphicsObject.h"

#include <dtkCore/dtkSmartPointer.h>

#include <QColor>
#include <QVector3D>

namespace mseg {
class SeedPointAnnotationData;

class MEDVIEWSEGMENTATIONPLUGIN_EXPORT SeedPointAnnotation : public medAnnotationGraphicsObject
{
    Q_OBJECT;
public:
    SeedPointAnnotation( QGraphicsItem * parent = 0 );
    virtual ~SeedPointAnnotation(void);

    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 ) MED_OVERRIDE;
    QRectF boundingRect(void) const MED_OVERRIDE;

    static medAnnotationGraphicsObject * s_create( );
    static QString s_description();


protected:
    void onSceneChanged(QGraphicsScene * scene) MED_OVERRIDE;
    void onSceneCameraChanged() MED_OVERRIDE;
    void onSceneOrientationChanged() MED_OVERRIDE;
    void initializeSceneCoordinates();
private:
    const SeedPointAnnotationData *seedPointAnnotationData() const;

    bool m_isInitialized;
};

} // namespace mseg

#endif // MSEGSEEDPOINTANNOTATION_H

