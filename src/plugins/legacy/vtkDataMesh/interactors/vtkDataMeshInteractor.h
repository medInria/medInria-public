#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include "vtkDataMeshPluginExport.h"

#include <medAbstractImageViewInteractor.h>

#include <vtkPointSet.h>

class vtkDataMeshInteractorPrivate;

class medAbstractData;
class dtkAbstractView;
class vtkLookupTable;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshInteractor: public medAbstractImageViewInteractor
{
    Q_OBJECT

public:
    vtkDataMeshInteractor(medAbstractView* parent);
    virtual ~vtkDataMeshInteractor();

    virtual QString description() const;
    virtual QString name() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setInputData(medAbstractData * data);

    bool edgeVisibility() const;
    QColor color() const;
    QString renderingType() const;
    QString attribute() const;
    QString lut() const;

    virtual QWidget* buildLayerWidget();
    virtual QWidget* buildToolBarWidget();
    virtual QWidget* buildToolBoxWidget();
    virtual QList<medAbstractParameterL*> linkableParameters();
    virtual QList<medBoolParameterL*> mouseInteractionParameters();

public slots:
    void setOpacity(double value);
    void setWindowLevel (QHash<QString,QVariant>);
    void setColor(const QString &color);
    void setColor(QColor color);
    void setMaxRange(double max);
    void setMinRange(double min);
    void setVisibility(bool visible);
    void setEdgeVisibility(bool visible);
    void setRenderingType(const QString &type);
    void setAttribute(const QString & attribute);

    void initWindowLevelParameters(double * range, int dataType);

    void setLut(const QString &lutName);

    virtual void removeData();
    virtual void setUpViewForThumbnail();
    virtual void moveToSlice  (int slice);

    virtual void updateWidgets();

protected:
    void updatePipeline ();
    void setLut(vtkLookupTable * lut);

    void setupParameters();
    void restoreParameters(QHash<QString, QString> parameters);

private:
    static QStringList dataHandled();

private slots:
    void updateSlicingParam();
    void updateRange();

private:
    vtkDataMeshInteractorPrivate * d;
};
