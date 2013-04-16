/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medAbstractAnnotationViewInteractor.h"

#include "v3dViewPluginExport.h"

#include <vtkAbstractWidget.h>

class v3dViewAnnotationInteractorPrivate;
class v3dView;
class v3dViewAnnotationInteractor;

// Helper class for v3dViewAnnotationInteractor
class V3DVIEWPLUGIN_EXPORT v3dViewAnnIntHelper {
public :
    v3dViewAnnIntHelper(v3dViewAnnotationInteractor * annInt);
    virtual ~v3dViewAnnIntHelper();
    virtual bool addAnnotation( medAnnotationData* annData ) = 0;
    virtual void removeAnnotation( medAnnotationData * annData ) = 0;
    virtual void annotationModified( medAnnotationData* annData ) = 0;
protected:
    v3dView * getV3dView();
    v3dViewAnnotationInteractor * m_v3dViewAnnInt;
};


//! Interface between annotations and the v3dview.
class V3DVIEWPLUGIN_EXPORT v3dViewAnnotationInteractor: public medAbstractAnnotationViewInteractor
{
    typedef medAbstractAnnotationViewInteractor BaseClass;

    Q_OBJECT

public:
    v3dViewAnnotationInteractor();
    virtual ~v3dViewAnnotationInteractor();

    //! Override dtkAbstractObject
    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void enable();
    virtual void disable();

    //! Override dtkAbstractViewInteractor.
    virtual bool isAutoEnabledWith ( dtkAbstractData * data );

    void setView(dtkAbstractView* view);

    static QString s_identifier();

protected:

    void onPropertySet(const QString& key, const QString& value);

    void onVisibilityPropertySet (const QString& value);

    v3dView * getV3dView();

    void initialize(medAbstractView * view, medAbstractData* data);

public slots:
    virtual void onDataModified(medAbstractData* data);

signals:

public slots:

    virtual bool onAddAnnotation( medAnnotationData * annItem );
    virtual void onRemoveAnnotation( medAnnotationData * annItem );

protected:
    friend class v3dViewAnnotationInteractorPrivate;
    friend class v3dViewAnnIntHelper;

private:
    v3dViewAnnotationInteractorPrivate *d;


};


// Inline this for speed.
inline v3dView * v3dViewAnnIntHelper::getV3dView()
{
    return m_v3dViewAnnInt->getV3dView();
}


