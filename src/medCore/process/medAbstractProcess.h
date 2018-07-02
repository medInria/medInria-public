/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcess_p.h>
#include <medAbstractData.h>

#include <medCoreExport.h>

class medAbstractProcessPrivate;

/**
 * Extending dtkAbstractProcess class to hold more specific information
 */
class MEDCORE_EXPORT medAbstractProcess : public dtkAbstractProcess
{
    Q_OBJECT

public:
    medAbstractProcess( medAbstractProcess * parent = NULL );
    medAbstractProcess(const medAbstractProcess& other);
    virtual ~medAbstractProcess();
    virtual void setInput ( medAbstractData *data, int channel = 0 ){}

    enum DataError
    {
        SUCCESS = DTK_SUCCEED,
        FAILURE = DTK_FAILURE,

        PIXEL_TYPE = 1 + SUCCESS * SUCCESS + FAILURE * FAILURE, //! Pixel type not yet implemented
        DIMENSION_3D,   //! Not a 3D volume
        DIMENSION_4D,   //! Not a 4D volume
        MESH_TYPE,      //! Not a mesh
        MESH_3D,        //! Not a 3D mesh
        MESH_4D,        //! Not a 4D mesh
        NO_MESH,        //! Input can not be a mesh
        DATA_SIZE,      //! Inputs must be the same size
        MISMATCHED_DATA_TYPES, //! Inputs must be the same type
        MISMATCHED_DATA_SIZES_ORIGIN_SPACING, //! Inputs should have the same size, origin, spacing
        MISMATCHED_DATA_SIZE,    //! Inputs should have the same size
        MISMATCHED_DATA_ORIGIN,  //! Inputs should have the same origin
        MISMATCHED_DATA_SPACING, //! Inputs should have the same spacing
        UNDEFINED,      //! Miscellanous
    };

public slots:
    virtual medAbstractData *output() = 0;
    virtual int update () = 0;

private:
    using dtkAbstractProcess::onCanceled;
    using dtkAbstractProcess::read;
    using dtkAbstractProcess::write;
    using dtkAbstractProcess::setInput;
    using dtkAbstractProcess::setData;
    //TODO rename our output methode
    //using dtkAbstractProcess::output;
    using dtkAbstractProcess::data;
    using dtkAbstractProcess::channelCount;

private:
    DTK_DECLARE_PRIVATE(medAbstractProcess)
    medAbstractProcessPrivate* d;

};

class MEDCORE_EXPORT medAbstractProcessPrivate : public dtkAbstractProcessPrivate
{
public:
    medAbstractProcessPrivate(medAbstractProcess *q = 0) : dtkAbstractProcessPrivate(q) {}
    medAbstractProcessPrivate(const medAbstractProcessPrivate& other) : dtkAbstractProcessPrivate(other) {}

public:
    virtual ~medAbstractProcessPrivate(void) {}
};

DTK_IMPLEMENT_PRIVATE(medAbstractProcess, dtkAbstractProcess)
