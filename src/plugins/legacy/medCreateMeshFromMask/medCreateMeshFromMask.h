/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medAbstractData.h>
#include <medAbstractProcessLegacy.h>
#include <medCreateMeshFromMaskPluginExport.h>

class medCreateMeshFromMaskPrivate;

class MEDCREATEMESHFROMMASKPLUGIN_EXPORT medCreateMeshFromMask : public medAbstractProcessLegacy
{
    Q_OBJECT

public:
    medCreateMeshFromMask();
    virtual ~medCreateMeshFromMask();

    virtual QString description() const;

    static bool registered();

public slots:

    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data, int channel = 0);

    //! Parameters are set through here, channel allows to handle multiple parameters
    void setParameter(double data, int channel);

    //! Method to actually start the filter
    int update();

    //! The output will be available through here
    medAbstractData *output();

    int getNumberOfTriangles();

private:
    medCreateMeshFromMaskPrivate *d;
};

dtkAbstractProcess *createmedCreateMeshFromMask();
