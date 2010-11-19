#include "ui_SimpleView.h"
#include "SimpleView.h"

// Qt
#include <QSettings>
#include <QDockWidget>
#include <QFileDialog>
#include <QProgressDialog>

// dicom net 
#include "dcmtkEchoScu.h"
#include "dcmtkFindScu.h"
#include "dcmtkMoveScu.h"
#include "dcmtkStoreScp.h"
#include "dcmtkStoreScu.h"
#include "dcmtkResultDataset.h"

// logger
#include "dcmtkLogger.h"
#include "LoggerFileOutput.h"
#include "LoggerConsoleOutput.h"
#include "LoggerWidgetOutput.h"
#include "LoggerWidget.h"

// utilities
#include "dcmtkDump.h"

#include <sstream>


//---------------------------------------------------------------------------------------------

// Constructor
SimpleView::SimpleView() 
{

  //initialize logger
  dcmtkLogger::startUp();
  
  // create and show LoggerWidget
  this->ui = new Ui_SimpleView;
  this->ui->setupUi(this);
  

  QStringList labels;
  labels << "Patient name" << "Description" << "ID/Number"  << "Modality/Type";
  ui->treeWidget->setHeaderLabels(labels);
  ui->treeWidget->setHeaderHidden(false);

  m_echoScu = new dcmtkEchoScu();
  m_findScu = new dcmtkFindScu();

  m_serverThread = new dcmtkStoreScp();
  m_sendThread = new dcmtkStoreScu();
  m_moveThread = new dcmtkMoveScu();

  m_shellOut = new LoggerConsoleOutput();
  m_fileOut = new LoggerFileOutput();
  m_loggerWidget = new LoggerWidget(this);
  m_widgetOut = new LoggerWidgetOutput(m_loggerWidget);

  connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(quit()));
  connect(this->ui->searchField, SIGNAL(returnPressed()), this, SLOT(findStudyLevel()));
  connect(this->ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(updateContextMenu(const QPoint&)));
  connect(this->ui->treeWidget, SIGNAL(itemExpanded ( QTreeWidgetItem*  )), this, SLOT(findSeriesLevel(QTreeWidgetItem *)));
  connect(this->ui->echoButton, SIGNAL(clicked()), this, SLOT(echo()));
  connect(this->ui->applySettingsButton, SIGNAL(clicked()), this, SLOT(applySettingsSlot()));
  connect(this->ui->cbFileTarget, SIGNAL(stateChanged(int)), this, SLOT(fileAppender(int)));
  connect(this->ui->cbShellTarget, SIGNAL(stateChanged(int)), this, SLOT(shellAppender(int)));
  connect(this->ui->cbWidgetTarget, SIGNAL(stateChanged(int)), this, SLOT(widgetAppender(int)));
  connect(this->ui->cbLogLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLogLevel(int)));
  connect(this->ui->serverRestartButton, SIGNAL(clicked()), this, SLOT(restartServer()));
  connect(this->ui->dirButton, SIGNAL(clicked()),this,SLOT(setSendDirectory()));
  connect(this->ui->dirbutton2, SIGNAL(clicked()), this, SLOT(setArchiveDirectory()));
  connect(this->ui->diredit2, SIGNAL(editingFinished()), this, SLOT(updateServerDir()));
  connect(this->ui->sendButton, SIGNAL(clicked()),this,SLOT(store()));
  connect(this->ui->addButton, SIGNAL(clicked()), this, SLOT(addConn()));
  connect(this->ui->connTableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(handleConnSelection()));
  connect(this->ui->peerTitleEdit, SIGNAL(textChanged(QString)), this, SLOT(inputChanged()));
  connect(this->ui->peerIPEdit, SIGNAL(textChanged(QString)), this, SLOT(inputChanged()));
  connect(this->ui->peerPortEdit, SIGNAL(textChanged(QString)), this, SLOT(inputChanged()));
  connect(m_findScu,SIGNAL(finished()), this, SLOT(fillTreeStudy()));

  progress = new QProgressDialog("Fetching data...", "Cancel", 0, 100);
  progress->setWindowModality(Qt::WindowModal);
  connect(m_moveThread,SIGNAL(progressed(int)), progress,SLOT(setValue(int)));
  connect(progress,SIGNAL(canceled()),m_moveThread,SLOT(sendCancelRequest()));

  retrieveSettings();
  setConnectionParams();

  //set logger defaults
  ui->cbWidgetTarget->setChecked(true);
  ui->cbLogLevel->setCurrentIndex(2);

  QDockWidget* loggerDock = new QDockWidget(this);
  loggerDock->setWidget(m_loggerWidget);
  this->addDockWidget(Qt::BottomDockWidgetArea, loggerDock);

  // start the threaded server
   startServer();

   ui->treeWidget->setFrameStyle(QFrame::NoFrame);
   ui->treeWidget->setAttribute(Qt::WA_MacShowFocusRect, false);
   ui->treeWidget->setUniformRowHeights(true);
   ui->treeWidget->setAlternatingRowColors(true);
   ui->treeWidget->setSortingEnabled(true);
   ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

   ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

};

//---------------------------------------------------------------------------------------------

SimpleView::~SimpleView()
{
  dcmtkLogger::shutDown();

  stopServer();
 
  delete m_findScu;
  delete m_echoScu;

  delete m_sendThread;
  delete m_moveThread;

  delete m_shellOut;
  delete m_fileOut;
  delete m_widgetOut;

  if(ui != NULL) delete ui;

}

//---------------------------------------------------------------------------------------------

void SimpleView::changeLogLevel(int index)
{
    LoggerLogLevel::LogLevel logLevel;
    switch(index)
    {
    case 0:
        logLevel = LoggerLogLevel::NOLOG;
        break;
    case 1:
        logLevel = LoggerLogLevel::ERRORLOG;
        break;
    case 2:
        logLevel = LoggerLogLevel::WARNINGLOG;
        break;
    case 3:
        logLevel = LoggerLogLevel::INFOLOG;
        break;
    case 4:
        logLevel = LoggerLogLevel::DEBUGLOG;
        break;
    default:
        logLevel = LoggerLogLevel::INFOLOG;
        break;
    }

    m_fileOut->setLogLevel(logLevel);
    m_shellOut->setLogLevel(logLevel);
    m_widgetOut->setLogLevel(logLevel);
}

//---------------------------------------------------------------------------------------------

void SimpleView::setConnectionParams()
{

    m_ourNode.setTitle(ui->ourTitleEdit->text().toStdString());
    m_ourNode.setIp(ui->ourIPEdit->text().toStdString());
    m_peerTitle = ui->peerTitleEdit->text().toStdString();
    m_peerIP    = ui->peerIPEdit->text().toStdString();
    
    try
    {
        m_peerPort  = ui->peerPortEdit->text().toInt();
    }
    catch(...)
    {
        ui->peerPortEdit->setText("");
    }

    try
    {
        m_ourNode.setPort(ui->ourPortEdit->text().toInt());
    }
    catch(...)
    {
        ui->ourPortEdit->setText("9999");
    }


    m_echoScu->setConnectionParams(m_peerTitle.c_str(), m_peerIP.c_str(), m_peerPort, m_ourNode.title().c_str(),m_ourNode.ip().c_str(),m_ourNode.port());
    m_serverThread->setConnectionParams(m_ourNode.title().c_str(),m_ourNode.ip().c_str(),m_ourNode.port());

}

//---------------------------------------------------------------------------------------------

// Action to be taken upon file open 
void SimpleView::findStudyLevel()
{
    // get selected nodes
    dcmtkContainer<dcmtkNode*>* selectedNodes = getSelectedNodes();

    QString patientName = ui->searchField->text();
    patientName.append("*");

    // clear previous results
    m_findScu->clearAllQueryAttributes();

    // set up search criteria
    m_findScu->setQueryLevel(dcmtkFindScu::STUDY);
    m_findScu->addQueryAttribute(0x0010,0x0010,patientName.toLatin1()); // patient name
    m_findScu->addQueryAttribute(0x0008,0x0030,"\0"); // study date
    m_findScu->addQueryAttribute(0x0008,0x0050,"\0"); // accession no
    m_findScu->addQueryAttribute(0x0008,0x0061,"\0"); // modalities in study
    m_findScu->addQueryAttribute(0x0008,0x0090,"\0"); // ref physician
    m_findScu->addQueryAttribute(0x0008,0x1030,"\0"); // study description
    m_findScu->addQueryAttribute(0x0010,0x0020,"\0"); // patient ID
    m_findScu->addQueryAttribute(0x0010,0x0030,"\0"); // patient BD
    m_findScu->addQueryAttribute(0x0010,0x0040,"\0"); // sex
    m_findScu->addQueryAttribute(0x0020,0x000D,"\0"); // studyInstanceUID
    m_findScu->addQueryAttribute(0x0020,0x0010,"\0"); // study ID
   

    // we repeat the find process with the data from all selected nodes
    dcmtkNode* selNode = selectedNodes->getFirst();
    while (selectedNodes->isValid())
    {
        QProgressDialog* progress = new QProgressDialog("Looking for images...", "Cancel", 0, 100);
        progress->setWindowModality(Qt::WindowModal);
        connect(m_findScu,SIGNAL(progressed(int)), progress,SLOT(setValue(int)));
        connect(progress,SIGNAL(canceled()),m_findScu,SLOT(sendCancelRequest()));
        progress->show();

        // do the work
        m_findScu->wait();
        m_findScu->setConnectionParams(selNode->title().c_str(),selNode->ip().c_str(),selNode->port(),
                                       m_ourNode.title().c_str(),m_ourNode.ip().c_str(),m_ourNode.port());
        m_findScu->start();
        
        selNode = selectedNodes->getNext();
    }


    // clean up
    delete selectedNodes;
}

//---------------------------------------------------------------------------------------------

void SimpleView::findSeriesLevel(QTreeWidgetItem * item)
{
    // check if its a top level item
    if (item->parent() != NULL) 
    {
        if (item->parent()->parent() != NULL)
        {
            findImageLevel(item); // forward to image level
            return;
        }
    } else return;

    // check if the item already has children (don't query again)
    if (item->child(0) != NULL) return;

    // retrieve data
    int nodeIndex = item->data(0,Qt::UserRole).toInt();
    QPoint tag = item->data(1,Qt::UserRole).toPoint();
    QString searchStr = item->data(2,Qt::UserRole).toString();
    dcmtkContainer<dcmtkNode*>* mainNodeCont = m_findScu->getNodeContainer();
    dcmtkConnectionData cdata;
    dcmtkNode* node = mainNodeCont->getAtPos(nodeIndex);
    cdata.title = node->title();
    cdata.ip = node->ip();
    cdata.port = node->port();
 
    // clear previous results
    dcmtkFindScu* findScu = new dcmtkFindScu;

    // set up search criteria
    findScu->setQueryLevel(dcmtkFindScu::SERIES);
    findScu->addQueryAttribute(tag.x(),tag.y(),searchStr.toLatin1()); // studyInstanceUID
    findScu->addQueryAttribute(0x0008,0x0021,"\0"); // series date
    findScu->addQueryAttribute(0x0008,0x0031,"\0"); // series time
    findScu->addQueryAttribute(0x0008,0x0060,"\0"); // series modality
    findScu->addQueryAttribute(0x0008,0x103E,"\0"); // series description
    findScu->addQueryAttribute(0x0018,0x0015,"\0"); // body part
    findScu->addQueryAttribute(0x0018,0x1030,"\0"); // protocol name
    findScu->addQueryAttribute(0x0018,0x5100,"\0"); // patient position
    findScu->addQueryAttribute(0x0020,0x000E,"\0"); // series instance UID
    findScu->addQueryAttribute(0x0020,0x0011,"\0"); // series number
    findScu->addQueryAttribute(0x0020,0x0052,"\0"); // frame of reference

    // do the work
    findScu->sendFindRequest(cdata.title.c_str(),cdata.ip.c_str(),cdata.port,m_ourNode.title().c_str(),m_ourNode.ip().c_str(),m_ourNode.port());

    // now extract information from datasets and build a visual list
    int sum = 0;
    dcmtkContainer<dcmtkNode*>* resNodeCont = findScu->getNodeContainer();
    dcmtkNode* myNode = resNodeCont->getFirst();
    
    if (resNodeCont->isValid())
    {
        dcmtkContainer<dcmtkResultDataset*>* resCont = myNode->getResultDatasetContainer();
        dcmtkResultDataset* resDs = resCont->getFirst();

        while (resCont->isValid())
        {
            // add result to list
            QTreeWidgetItem *pItem = new QTreeWidgetItem(item);
            pItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

            pItem->setData(0,Qt::UserRole, nodeIndex); //forwarding node index
            pItem->setData(1,Qt::UserRole, QPoint(0x0020,0x000E)); // tag
            pItem->setData(2,Qt::UserRole, QString(resDs->getSeriesInstanceUID())); // search value

            pItem->setText(0,">>");
            pItem->setText(1,resDs->findKeyValue(0x0008,0x103E));
            pItem->setText(2,resDs->findKeyValue(0x0020,0x0011)); // series number
            item->addChild(pItem);
            resDs = resCont->getNext();
        }
        sum = resCont->size();
        myNode = resNodeCont->getNext();
    }

    // print to logwindow
    printResults(sum,"series");

    // clean up
    delete findScu;
}

//---------------------------------------------------------------------------------------------

void SimpleView::findImageLevel(QTreeWidgetItem * item)
{
    // check if the item already has children (don't query again)
    if (item->child(0) != NULL) return;

    // retrieve data
    int nodeIndex = item->data(0,Qt::UserRole).toInt(); // node index
    QString searchStr = item->data(2,Qt::UserRole).toString(); // search value
    dcmtkContainer<dcmtkNode*>* mainNodeCont = m_findScu->getNodeContainer();
    
    dcmtkConnectionData cdata;
    dcmtkNode* myNode = mainNodeCont->getAtPos(nodeIndex);
    cdata.title = myNode->title();
    cdata.ip = myNode->ip();
    cdata.port = myNode->port();

    // clear previous results
    dcmtkFindScu* findScu = new dcmtkFindScu;

    // set up search criteria
    findScu->setQueryLevel(dcmtkFindScu::IMAGE);
    findScu->addQueryAttribute(0x0020,0x000E,searchStr.toLatin1()); // series instance UID
    findScu->addQueryAttribute(0x0008,0x0008,"\0"); // image type
    findScu->addQueryAttribute(0x0008,0x0012,"\0"); // instance creation date
    findScu->addQueryAttribute(0x0008,0x0013,"\0"); // instance creation time
    findScu->addQueryAttribute(0x0008,0x0016,"\0"); // SOP class UID
    findScu->addQueryAttribute(0x0008,0x0018,"\0"); // SOP instance UID
    findScu->addQueryAttribute(0x0008,0x0022,"\0"); // image date
    findScu->addQueryAttribute(0x0008,0x0032,"\0"); // image time
    findScu->addQueryAttribute(0x0020,0x0012,"\0"); // acquisition number
    findScu->addQueryAttribute(0x0020,0x000D,"\0"); // study instance UID
    findScu->addQueryAttribute(0x0020,0x0013,"\0"); // instance time
    findScu->addQueryAttribute(0x0020,0x0032,"\0"); // image position patient
    findScu->addQueryAttribute(0x0020,0x0037,"\0"); // image orientation patient
    findScu->addQueryAttribute(0x0020,0x1041,"\0"); // slice location
    findScu->addQueryAttribute(0x0028,0x0008,"\0"); // number of frames
    
    // do the work
    findScu->sendFindRequest(cdata.title.c_str(),cdata.ip.c_str(),cdata.port,m_ourNode.title().c_str(),m_ourNode.ip().c_str(),m_ourNode.port());

    // now extract information from datasets and build a visual list
    int sum = 0;
    dcmtkContainer<dcmtkNode*>* resNodeCont = findScu->getNodeContainer();
    dcmtkNode* node = resNodeCont->getFirst();

    if (resNodeCont->isValid())
    {
        dcmtkContainer<dcmtkResultDataset*>* resCont = node->getResultDatasetContainer();
        dcmtkResultDataset* resDs = resCont->getFirst();

        while (resCont->isValid())
        {
            // add result to list
            QTreeWidgetItem *pItem = new QTreeWidgetItem(item);

            pItem->setData(0,Qt::UserRole, nodeIndex); //forwarding node index
            pItem->setData(1,Qt::UserRole, QPoint(0x0008,0x0018)); // tag
            pItem->setData(2,Qt::UserRole, QString(resDs->getSOPInstanceUID())); // search value

            pItem->setText(0,">>");
            pItem->setText(2,resDs->findKeyValue(0x0020,0x0012)); // aqu. number
            pItem->setText(3,resDs->findKeyValue(0x0008,0x0008)); // image type
            item->addChild(pItem);
            resDs = resCont->getNext();
        }
        sum = resCont->size();
    }

    // print to logwindow
    printResults(sum,"images");

    // clean up
    delete findScu;
}

//---------------------------------------------------------------------------------------------

void SimpleView::move(QTreeWidgetItem * item, int column) 
{
    dcmtkContainer<dcmtkNode*>* mainNodeCont =  m_findScu->getNodeContainer();

    // retrieve data
    int nodeIndex = item->data(0,Qt::UserRole).toInt();
    QPoint tag = item->data(1,Qt::UserRole).toPoint();
    QString searchStr = item->data(2,Qt::UserRole).toString();

    ui->logWindow->append(tr("Fetching data..."));
    ui->logWindow->repaint();

    // set up search criteria
    m_moveThread->clearAllQueryAttributes();

    // find out which query level should be used
    int elem = tag.y();
    switch(elem)
    {
    case 0x0018:
        m_moveThread->setQueryLevel(dcmtkMoveScu::IMAGE);
        break;

    case 0x000E:
        m_moveThread->setQueryLevel(dcmtkMoveScu::SERIES);
        break;

    case 0x000D:
        m_moveThread->setQueryLevel(dcmtkMoveScu::STUDY);
        break;

    default:
        ui->logWindow->append(tr("Could not determine query level."));
    }



    m_moveThread->addQueryAttribute(tag.x(), tag.y(), searchStr.toLatin1());

    // send the move request using the search crits
    dcmtkNode* myNode = mainNodeCont->getAtPos(nodeIndex);
    
    progress->show();
    m_moveThread->wait();
    m_moveThread->setConnectionParams(myNode->title().c_str(),myNode->ip().c_str(),myNode->port(),m_ourNode.title().c_str(),m_ourNode.ip().c_str(),m_ourNode.port());
    m_moveThread->start();

}

//---------------------------------------------------------------------------------------------

void SimpleView::echo()
{
    // update params
    setConnectionParams();

    // now send the echo
    if ( m_echoScu->sendEchoRequest() == 0 )
    {
        ui->logWindow->append(tr("Connection verified successfully"));
    }
    else
        ui->logWindow->append(tr("No response from peer. Check your connection"));

}

//---------------------------------------------------------------------------------------------

void SimpleView::store()
{
    // set the current node params first
    dcmtkConnectionData* cb = &(m_nodes.at(ui->sendToNodeCB->currentIndex()));

    m_sendThread->setConnectionParams(cb->title.c_str(), cb->ip.c_str(), cb->port, m_ourNode.title().c_str(),m_ourNode.ip().c_str(),m_ourNode.port());
    
    QDir testDir;
    testDir.setPath(ui->directoryLE->text());
    if ( testDir.isReadable() )
    {
        m_sendThread->setScanDirectory(ui->directoryLE->text().toLatin1());
        m_sendThread->start();
    }
    else
    {
        ui->logWindow->append("This is not a valid import directory!");
    }
}

//---------------------------------------------------------------------------------------------

void SimpleView::setSendDirectory()
{
    QDir selDir;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    if (dialog.exec())
    {
        selDir = dialog.directory();
        this->ui->directoryLE->setText(selDir.path());
    }
}

//---------------------------------------------------------------------------------------------

void SimpleView::applySettingsSlot()
{
    storeSettings(1);
    setConnectionParams();
    restartServer();
}

//---------------------------------------------------------------------------------------------

void SimpleView::storeSettings(int type)
{
    QSettings settings("INRIA", "DCMTKTOOL");

    std::vector<dcmtkConnectionData>::iterator iter;
    int count = 0;
    QString countString;

    switch(type)
    {
    case 0:
        settings.beginGroup("Connection");
        for( iter = m_nodes.begin(); iter != m_nodes.end(); iter++)
        {
            countString.setNum(count);
            QString port;
            port.setNum((*iter).port);
            settings.setValue(QString("PEER_AET" + countString), QString::fromStdString((*iter).title));
            settings.setValue(QString("PEER_IP" + countString), QString::fromStdString((*iter).ip));
            settings.setValue(QString("PEER_PORT" + countString), port);
            count++;
        }
        // remove all items coming after the last one
        countString.setNum(count);
        while (settings.contains(QString("PEER_IP" + countString)))
        {
            settings.remove(QString("PEER_AET" + countString));
            settings.remove(QString("PEER_IP" + countString));
            settings.remove(QString("PEER_PORT" + countString));
        }

        settings.endGroup();
        break;
    case 1:
        settings.beginGroup("Connection");
        settings.setValue("OUR_AET", ui->ourTitleEdit->text());
        settings.setValue("OUR_IP", ui->ourIPEdit->text());
        settings.setValue("OUR_PORT", ui->ourPortEdit->text());
        settings.endGroup();
        break;
    case 2:
        settings.beginGroup("Server");
        settings.setValue("ARCHIVE_DIR", ui->diredit2->text());
        settings.endGroup();
        break;
    default:
        std::cout << "switch error" << std::endl;
        break;
    }
}

//---------------------------------------------------------------------------------------------

void SimpleView::retrieveSettings()
{
    QString ourTitle, ourIP, ourPort;
    QString peerTitle, peerIP, peerPort;
    QString archiveDir;
    int count = 0;
    QString countString;
    countString.setNum(count);
    QSettings settings("INRIA", "DCMTKTOOL");
    settings.beginGroup("Connection");
    while (settings.contains(QString("PEER_IP" + countString)))
    {
        peerTitle = settings.value("PEER_AET" + countString).value<QString>();
        peerIP = settings.value("PEER_IP" + countString).value<QString>();
        peerPort = settings.value("PEER_PORT" + countString).value<QString>();
        this->addConnection(peerTitle, peerIP, peerPort);
        count++;
        countString.setNum(count);
    }

    ourTitle = settings.value("OUR_AET").value<QString>();
    ourIP = settings.value("OUR_IP").value<QString>();
    ourPort = settings.value("OUR_PORT").value<QString>();
    settings.endGroup();
    
    settings.beginGroup("Server");
    archiveDir = settings.value("ARCHIVE_DIR").value<QString>();
    settings.endGroup();

    ui->peerTitleEdit->setText(peerTitle);
    ui->peerIPEdit->setText(peerIP);
    ui->peerPortEdit->setText(peerPort);

    // apply only if none-empty
    if(!ourTitle.isEmpty()) ui->ourTitleEdit->setText(ourTitle);
    if(!ourIP.isEmpty()) ui->ourIPEdit->setText(ourIP);
    if(!ourPort.isEmpty()) ui->ourPortEdit->setText(ourPort);
    if(!archiveDir.isEmpty()) 
    {
        ui->diredit2->setText(archiveDir);
        updateServerDir();
    }

}

//---------------------------------------------------------------------------------------------

void SimpleView::fileAppender(int state)
{
 switch(state)
 {
 case 0:
      BaseLogger::removeOutput(m_fileOut);
      break;
 case 2:
      BaseLogger::addOutput(m_fileOut);
      break;
 default:
     std::cout << "switch error" << std::endl;
      break;
 }
}

//---------------------------------------------------------------------------------------------

void SimpleView::shellAppender(int state)
{
 switch(state)
 {
 case 0:
      BaseLogger::removeOutput(m_shellOut);
      break;
 case 2:
      BaseLogger::addOutput(m_shellOut);
      break;
 default:
     std::cout << "switch error" << std::endl;
      break;
 }
}

//---------------------------------------------------------------------------------------------

void SimpleView::widgetAppender(int state)
{

 switch(state)
 {
 case 0:
      BaseLogger::removeOutput(m_widgetOut);
      break;
 case 2:
      BaseLogger::addOutput(m_widgetOut);
      break;
 default:
     std::cout << "switch error" << std::endl;
      break;
 }

}

//---------------------------------------------------------------------------------------------

void SimpleView::startServer()
{
    ui->logWindow->append(tr("Server started."));
    m_serverThread->start();
}

//---------------------------------------------------------------------------------------------

void SimpleView::stopServer()
{
    if (m_serverThread->isRunning())
    {
        m_serverThread->stopService();
        m_serverThread->exit();
        m_serverThread->wait();
    }

    ui->logWindow->append(tr("Server stopped."));
}

//---------------------------------------------------------------------------------------------

void SimpleView::restartServer()
{
    stopServer();
    startServer();
}

//---------------------------------------------------------------------------------------------

void SimpleView::quit()
{
    qApp->quit();
}

//---------------------------------------------------------------------------------------------

void SimpleView::addConn()
{
    if (ui->addButton->text() == "Add")
    {
        addConnection(ui->peerTitleEdit->text(), ui->peerIPEdit->text(), ui->peerPortEdit->text());
    }
    else
    {
        removeConnection(ui->connTableWidget->currentRow());
    }
    storeSettings(0);
}

//---------------------------------------------------------------------------------------------

void SimpleView::addConnection(QString peer, QString ip, QString port)
{
    int row = ui->connTableWidget->rowCount();
    ui->connTableWidget->insertRow(row);
    ui->connTableWidget->setColumnCount(3);
    ui->connTableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    QStringList horHeaders;
    horHeaders << "AET" << "IP" << "PORT";
    ui->connTableWidget->setHorizontalHeaderLabels(horHeaders);

    QTableWidgetItem *aetItem = new QTableWidgetItem(peer);
    ui->connTableWidget->setItem(row, 0, aetItem);
    QTableWidgetItem *ipItem = new QTableWidgetItem(ip);
    ui->connTableWidget->setItem(row, 1, ipItem);
    QTableWidgetItem *portItem = new QTableWidgetItem(port);
    ui->connTableWidget->setItem(row, 2, portItem);

    dcmtkConnectionData cdata;
    cdata.title = peer.toStdString();
    cdata.ip    = ip.toStdString();
    cdata.port  = port.toInt();

    m_nodes.push_back(cdata);

    QListWidgetItem* myItem = new QListWidgetItem(peer,ui->nodeSelectionLW);
    myItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    myItem->setCheckState(Qt::Unchecked);

    // add available dicom nodes to cb, for the moment only one..
    ui->sendToNodeCB->addItem(peer);

}

//---------------------------------------------------------------------------------------------

void SimpleView::removeConnection(int index)
{
    ui->connTableWidget->removeRow(index);
    m_nodes.erase(m_nodes.begin()+index);
    ui->nodeSelectionLW->takeItem(index);
    ui->sendToNodeCB->removeItem(index);
}

//---------------------------------------------------------------------------------------------

void SimpleView::handleConnSelection()
{
    int row = this->ui->connTableWidget->currentRow();

    if ( row < (int)m_nodes.size())
    {
        ui->peerTitleEdit->setText(QString::fromStdString(m_nodes.at(row).title));
        ui->peerIPEdit->setText(QString::fromStdString(m_nodes.at(row).ip));
        QString mystring;
        ui->peerPortEdit->setText(mystring.setNum(m_nodes.at(row).port));

    }

    //change add button to remove
    ui->addButton->setText("Remove");
}

//---------------------------------------------------------------------------------------------

void SimpleView::inputChanged()
{
    ui->addButton->setText("Add");
}

//---------------------------------------------------------------------------------------------

dcmtkContainer<dcmtkNode*>* SimpleView::getSelectedNodes()
{
    //build selected node container
    dcmtkContainer<dcmtkNode*>* selectedNodes = new dcmtkContainer<dcmtkNode*>;
    for(int i=0; i < ui->nodeSelectionLW->count(); i++)
    {
      QListWidgetItem *item = ui->nodeSelectionLW->item(i);
      if( item->checkState() == Qt::Checked)
      {
          dcmtkNode* node = new dcmtkNode;
          node->setTitle(m_nodes.at(i).title);
          node->setIp(m_nodes.at(i).ip);
          node->setPort(m_nodes.at(i).port);
          selectedNodes->add(node);
      }
    }
    return selectedNodes;
}

//---------------------------------------------------------------------------------------------

void SimpleView::printResults(int sum, const char* type)
{
    QString number;
    number.setNum(sum);
    QString stype = type;
    ui->logWindow->append(number + " " + stype + " found.");
}

//---------------------------------------------------------------------------------------------

void SimpleView::fillTreeStudy()
{
    QString concatStudInstUID;

    // now extract information from datasets and build a visual list
    int sum = 0;
    ui->treeWidget->clear();
    dcmtkContainer<dcmtkNode*>* resNodeCont = m_findScu->getNodeContainer();
    dcmtkNode* myNode = resNodeCont->getFirst();

    while (resNodeCont->isValid())
    {
        dcmtkContainer<dcmtkResultDataset*>* resCont = myNode->getResultDatasetContainer();
        dcmtkResultDataset* resDs = resCont->getFirst();

        // add the root node containing the name of the DICOM NODE
        QTreeWidgetItem *topLevelItem = new QTreeWidgetItem();
        ui->treeWidget->addTopLevelItem(topLevelItem);
        topLevelItem->setText(0,myNode->title().c_str());

        while ( resCont->isValid())
        {
            // add result to list
            QTreeWidgetItem *pItem = new QTreeWidgetItem(topLevelItem);
            pItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

            pItem->setData(0,Qt::UserRole, resNodeCont->index()); //node index
            pItem->setData(1,Qt::UserRole, QPoint(0x0020,0x000D)); // tag
            pItem->setData(2,Qt::UserRole, QString(resDs->getStudyInstanceUID())); // search value
            pItem->setText(0,resDs->findKeyValue(0x0010,0x0010)); // patient name
            ui->treeWidget->insertTopLevelItem(0,pItem);

            // concatenate all studyInstUIDs belonging to this node
            concatStudInstUID = QString(resDs->getStudyInstanceUID()) +"\\"+ concatStudInstUID;

            resDs = resCont->getNext();
        }
        // now we add the list of studies to the top-level-item
        topLevelItem->setData(0,Qt::UserRole, resNodeCont->index()); //node index
        topLevelItem->setData(1,Qt::UserRole, QPoint(0x0020,0x000D)); // tag (studyInstanceUID)
        topLevelItem->setData(2,Qt::UserRole, concatStudInstUID); // search value
        topLevelItem->setExpanded(true);

        sum = sum + resCont->size();
        myNode = resNodeCont->getNext();

    }

    // print to logwindow
    printResults(sum,"studies");

}
//---------------------------------------------------------------------------------------------

void SimpleView::setArchiveDirectory()
{
    QDir selDir;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    if (dialog.exec())
    {
        selDir = dialog.directory();
        this->ui->diredit2->setText(selDir.path());
        updateServerDir();
    }

}

//---------------------------------------------------------------------------------------------

void SimpleView::updateServerDir()
{
    m_serverThread->setStorageDirectory(ui->diredit2->text().latin1());
    restartServer();
    storeSettings(2);
}

//---------------------------------------------------------------------------------------------

void SimpleView::updateContextMenu(const QPoint& point)
{
    QModelIndex index = this->ui->treeWidget->indexAt(point);

    if(!index.isValid())
        return;

    QMenu menu(this);
    menu.addAction("Import", this, SLOT(moveSelectedItems()));
    menu.exec(ui->treeWidget->mapToGlobal(point));
}

//---------------------------------------------------------------------------------------------

void SimpleView::moveSelectedItems()
{
    QList<QTreeWidgetItem*> itemList = ui->treeWidget->selectedItems();

    for (int i = 0; i < itemList.size(); i++) 
    {
        queuedMove(itemList.at(i));
    }
    m_moveThread->start();

}


//---------------------------------------------------------------------------------------------

void SimpleView::queuedMove(QTreeWidgetItem* item)
{
    dcmtkContainer<dcmtkNode*>* mainNodeCont =  m_findScu->getNodeContainer();

    // retrieve data
    int nodeIndex = item->data(0,Qt::UserRole).toInt();
    QPoint tag = item->data(1,Qt::UserRole).toPoint();
    QString searchStr = item->data(2,Qt::UserRole).toString();

    // send the move request using the search crits
    dcmtkNode* myNode = mainNodeCont->getAtPos(nodeIndex);

    m_moveThread->setConnectionParams(myNode->title().c_str(),myNode->ip().c_str(),myNode->port(),
                                      m_ourNode.title().c_str(),m_ourNode.ip().c_str(),m_ourNode.port());
    dcmtkNode peerNode(*myNode);
    dcmtkNode moveTarget(m_ourNode);

    m_moveThread->addRequestToQueue(tag.x(), tag.y(), searchStr.toLatin1(), peerNode, moveTarget);

}

//---------------------------------------------------------------------------------------------
