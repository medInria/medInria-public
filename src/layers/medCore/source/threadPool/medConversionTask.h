/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

//#include <QObject>

#include <QMap>
#include <QRunnable>
#include <QUuid>


#include <medAbstractData.h>
#include <medDataIndex.h>

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medNotifSys.h>

class medConversionTask : public QRunnable//, QObject
{
    //Q_OBJECT

public:
    medConversionTask(QMap<medDataIndex, dtkSmartPointer<medAbstractData> > & indexToData) : m_IndexToData(indexToData), m_notif(nullptr), m_bImport(false), m_bExport(false), m_data(nullptr) {}
    virtual ~medConversionTask();


    void run() override;

    bool importTask(medAbstractData &data, QString path, QUuid uuid = QUuid());
    bool exportTask(medAbstractData &data, QString path, QUuid uuid = QUuid());

    bool m_bImport;
    bool m_bExport;
    QString m_path;
    QUuid   m_uuid;
    QMap<medDataIndex, dtkSmartPointer<medAbstractData> > & m_IndexToData;
    medAbstractData * m_data;

    medUsrNotif * m_notif;
};



