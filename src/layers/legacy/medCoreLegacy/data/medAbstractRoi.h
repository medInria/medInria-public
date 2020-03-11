#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractObject.h>

#include <QUndoStack>

#include <medAbstractView.h>
#include <medCoreLegacyExport.h>

class medAbstractRoiPrivate;

class RoiStatistics
{
public:
    double max;
    double min;
    double mean;
    double sum;
    double std;
    double area;
    double perimeter;
};

class MEDCORELEGACY_EXPORT medAbstractRoi : public dtkAbstractObject
{
    Q_OBJECT

public:
    medAbstractRoi( dtkAbstractObject * parent = nullptr );
    virtual ~medAbstractRoi();

    virtual void Off()=0;
    virtual void On()=0;
    virtual bool isVisible()=0;
    virtual void forceInvisibilityOn()=0;
    virtual void forceInvisibilityOff()=0;

    virtual QString info()=0;
    virtual QString type()=0;

    unsigned int getIdSlice() const;
    void setIdSlice(unsigned int idSlice);

    unsigned char getOrientation();
    void setOrientation(unsigned char orientation);

    bool isSelected();
    virtual void computeRoiStatistics() = 0;
    virtual void setRoiStatistics(RoiStatistics s);    

    virtual bool canRedo() = 0;
    virtual bool canUndo() = 0;

    void setMasterRoi(bool);
    bool isMasterRoi() const;

public slots:
    virtual void select();
    virtual void unselect();
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual void saveState() = 0;
    virtual bool copyROI(medAbstractView *) = 0;
    virtual medAbstractRoi * getCopy(medAbstractView *) = 0;
    virtual QList<medAbstractRoi*> * interpolate(medAbstractRoi*) = 0;
    
signals:
    void selected();
    void stateSaved();
    
private:
    virtual void setRightColor() = 0;
    medAbstractRoiPrivate * d;
};

class MEDCORELEGACY_EXPORT RoiUndoRedo : public QUndoCommand
 {
 public:
     RoiUndoRedo(medAbstractRoi *roi, const QString &text)
         : m_roi(roi)
     {
         setText(text);
     }
     virtual void undo()
     {
         m_roi->undo();
     }
     virtual void redo()
     {
         m_roi->redo();
     }

 private:
    medAbstractRoi *m_roi;
 };
