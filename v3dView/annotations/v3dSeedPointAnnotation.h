#pragma once

#include "v3dViewPluginExport.h"

#include "medAbstractAnnotationRepresentation.h"

#include <dtkCore/dtkSmartPointer.h>

#include <QColor>
#include <QVector3D>

namespace mseg {
class SeedPointAnnotationData;

class V3DVIEWPLUGIN_EXPORT SeedPointAnnotation : public medAbstractAnnotationRepresentation
{
    Q_OBJECT;
    typedef medAbstractAnnotationRepresentation BaseClass;

public:
    SeedPointAnnotation( );
    virtual ~SeedPointAnnotation(void);

    static medAbstractAnnotationRepresentation * s_create( );
    static QString s_description();
    static QString s_identifier();


protected:
    void initializeSceneCoordinates();
    void onDataModified() MED_OVERRIDE;

private:
    const SeedPointAnnotationData *seedPointAnnotationData() const;

    bool m_isInitialized;
    vtkSmartPointer< vtkProp > m_prop;
};

} // namespace mseg



