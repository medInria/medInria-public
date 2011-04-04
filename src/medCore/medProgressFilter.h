#ifndef MEDPROGRESSFILTER_H
#define MEDPROGRESSFILTER_H

#include <QtCore/QObject>

#include "medCore/medCoreExport.h"

/**
* @class  medProgressFilter
* @author John Stark
* @brief  Allow the progress message sent by a child process to be transformed linearly to a range set by the parent.
*   
* both the parent and child processes should implement a progressed(int) signal.
*/
class MEDCORE_EXPORT medProgressFilter : public QObject
{
    Q_OBJECT;

public:
    /**  Constructor */
    medProgressFilter(QObject* parent, QObject* child, qreal startPercent = 0, qreal endPercent = 100);

    /**  Destructor */
    virtual ~medProgressFilter();

    void setRange( qreal startPercent, qreal endPercent );
    qreal startPercent () const { return _startPercent; }
    qreal endPercent () const   { return _endPercent; }

signals:
    /** Progress to send to the parent process, in the range _start to _end*/
    void progressed(int);

protected slots :
    /** Progress received from the child process, in the range 0 to 100 percent.*/
    void childProgressed(int percent);

private:
protected:
    qreal _startPercent;
    qreal _endPercent;
    int   _currentProgress;
};

#endif // MEDPROGRESSFILTER_H
