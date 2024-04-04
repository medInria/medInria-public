/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#ifndef DCMFIFOMOVER_H
#define DCMFIFOMOVER_H

#include <QMap>
#include <QObject>

#include <dcmMoveScu.h>

#include <medAbstractSource.h>

class QtDcmAPHP;

class dcmFifoMover : public QObject
{
    Q_OBJECT
public:

    dcmFifoMover();
    ~dcmFifoMover() override;

public slots:
    void processing();
    void addRequest(int requestId, dcmMoveScu *mover);
    void stopProcessing();

signals:
    void sendPending(const int requestId, medAbstractSource::eRequestStatus status);

private:
    QMap<int, dcmMoveScu*> m_RequestIdMap;
    bool m_MustRun;

};

/**
 * @fn void dcmFifoMover::processing()
 * @brief retrieve a thread (an instance of dcmMoveScu) using the request identifier and then call the DCMTK's MOVE request from it
*/

/**
 * @fn void dcmFifoMover::addRequest(int requestId, dcmMoveScu *mover)
 * @brief allow to link the dcmMoveScu thread instance to the request identifier
*/

/**
 * @fn void dcmFifoMover::stopProcessing()
 * @brief indicate that the process should stop
*/

#endif //DCMFIFOMOVER_H
