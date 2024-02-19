/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <thread>

#include "dcmFifoMover.h"

dcmFifoMover::dcmFifoMover() : QObject()
{
    m_MustRun = false;
}

dcmFifoMover::~dcmFifoMover()
{
    m_MustRun = false;
}


void dcmFifoMover::processing()
{
    m_MustRun = true;

    while(m_MustRun)
    {
        if(m_RequestIdMap.isEmpty())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else
        {
            int key = m_RequestIdMap.firstKey();
            auto mover = m_RequestIdMap.take(key);
            mover->start();

            while(mover->isRunning() && m_MustRun)
            {
                for (auto other : m_RequestIdMap.keys())
                {
                    emit sendPending(other, medAbstractSource::eRequestStatus::pending);
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            if (mover->isRunning())
            {
                mover->quit();
                mover->wait();
            }
            mover->deleteLater();
        }
    }
    for (auto mover : m_RequestIdMap)
    {
        mover->quit();
        mover->wait();
        mover->deleteLater();
    }
}

void dcmFifoMover::addRequest(int requestId, dcmMoveScu *mover)
{
    m_RequestIdMap[requestId] = mover;
}

void dcmFifoMover::stopProcessing()
{
    m_MustRun = false;
}
