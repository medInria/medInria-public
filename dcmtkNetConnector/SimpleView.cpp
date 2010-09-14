#include "ui_SimpleView.h"
#include "SimpleView.h"
#include "ServerThread.h"
#include "SendThread.h"

#include <QSettings>
#include <QDockWidget>
#include <QFileDialog>

#include "dcmtkEchoScu.h"
#include "dcmtkFindScu.h"
#include "dcmtkMoveScu.h"
#include "dcmtkFindDataset.h"

#include "Logger.h"
#include "LoggerFileOutput.h"
#include "LoggerConsoleOutput.h"
#include "LoggerWidgetOutput.h"
#include "LoggerWidget.h"

//---------------------------------------------------------------------------------------------

// Constructor
SimpleView::SimpleView() 
{
  //initialize logger
  Logger::startUp();

  this->ui = new Ui_SimpleView;
  this->ui->setupUi(this);

  m_echoScu = new dcmtkEchoScu();
  m_findScu = new dcmtkFindScu();
  m_moveScu = new dcmtkMoveScu();

  
  m_serverThread = new ServerThread();
  m_sendThread = new SendThread();

  m_shellOut = new LoggerConsoleOutput();
  m_fileOut = new LoggerFileOutput();
  m_loggerWidget = new LoggerWidget(this);
  m_widgetOut = new LoggerWidgetOutput(m_loggerWidget);

  connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(quit()));
  connect(this->ui->searchField, SIGNAL(returnPressed()), this, SLOT(findStudyLevel()));
  connect(this->ui->treeWidget, SIGNAL(itemDoubleClicked ( QTreeWidgetItem* , int )), this, SLOT(move(QTreeWidgetItem *, int)));
  connect(this->ui->treeWidget, SIGNAL(itemExpanded ( QTreeWidgetItem*  )), this, SLOT(findSeriesLevel(QTreeWidgetItem *)));
  connect(this->ui->echoButton, SIGNAL(clicked()), this, SLOT(echo()));
  connect(this->ui->applySettingsButton, SIGNAL(clicked()), this, SLOT(applySettingsSlot()));
  connect(this->ui->cbFileTarget, SIGNAL(stateChanged(int)), this, SLOT(fileAppender(int)));
  connect(this->ui->cbShellTarget, SIGNAL(stateChanged(int)), this, SLOT(shellAppender(int)));
  connect(this->ui->cbWidgetTarget, SIGNAL(stateChanged(int)), this, SLOT(widgetAppender(int)));
  connect(this->ui->cbLogLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLogLevel(int)));
  connect(this->ui->serverRestartButton, SIGNAL(clicked()), this, SLOT(restartServer()));
  connect(this->ui->dirButton, SIGNAL(clicked()),this,SLOT(setSendDirectory()));
  connect(this->ui->sendButton, SIGNAL(clicked()),this,SLOT(store()));
  connect(this->ui->addButton, SIGNAL(clicked()), this, SLOT(addConn()));
  connect(this->ui->connTableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(handleConnSelection()));
  connect(this->ui->peerTitleEdit, SIGNAL(textChanged(QString)), this, SLOT(inputChanged()));
  connect(this->ui->peerIPEdit, SIGNAL(textChanged(QString)), this, SLOT(inputChanged()));
  connect(this->ui->peerPortEdit, SIGNAL(textChanged(QString)), this, SLOT(inputChanged()));


  retrieveSettings();
  setConnectionParams();

  //set logger defaults
  ui->cbWidgetTarget->setChecked(true);
  ui->cbLogLevel->setCurrentIndex(2);

  // create and show LoggerWidget
  QDockWidget* loggerDock = new QDockWidget(this);
  loggerDock->setWidget(m_loggerWidget);
  this->addDockWidget(Qt::BottomDockWidgetArea, loggerDock);

  // start the threaded server
  startServer();

};

//---------------------------------------------------------------------------------------------

SimpleView::~SimpleView()
{
  delete m_findScu;
  delete m_moveScu;
  delete m_echoScu;

  delete m_serverThread;
  delete m_sendThread;

  delete m_shellOut;
  delete m_fileOut;
  delete m_widgetOut;
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

    m_ourTitle  = ui->ourTitleEdit->text().toStdString();
    m_ourIP     = ui->ourIPEdit->text().toStdString();
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
        m_ourPort   = ui->ourPortEdit->text().toInt();
    }
    catch(...)
    {
        ui->ourPortEdit->setText("100");
    }


    m_echoScu->setConnectionParams(m_peerTitle.c_str(), m_peerIP.c_str(), m_peerPort, m_ourTitle.c_str(), m_ourIP.c_str(), m_ourPort);
    m_serverThread->setConnectionParams(m_ourTitle.c_str(), m_ourIP.c_str(), m_ourPort);

}

//---------------------------------------------------------------------------------------------

// Action to be taken upon file open 
void SimpleView::findStudyLevel()
{
    // get selected nodes
    dcmtkNodeContainer* selectedNodes = getSelectedNodes();

    QString patientName = ui->searchField->text();
    patientName.append("*");

    // clear previous results
    m_findScu->clearAllQueryAttributes();

    // set up search criteria
    m_findScu->setQueryLevel(dcmtkFindScu::STUDY);
    m_findScu->addQueryAttribute("0010","0010",patientName.toLatin1()); // patient name
    m_findScu->addQueryAttribute("0008","0030","\0"); // study date
    m_findScu->addQueryAttribute("0008","0050","\0"); // accession no
    m_findScu->addQueryAttribute("0008","0061","\0"); // modalities in study
    m_findScu->addQueryAttribute("0008","0090","\0"); // ref physician
    m_findScu->addQueryAttribute("0008","1030","\0"); // study description
    m_findScu->addQueryAttribute("0010","0020","\0"); // patient ID
    m_findScu->addQueryAttribute("0010","0030","\0"); // patient BD
    m_findScu->addQueryAttribute("0010","0040","\0"); // sex
    m_findScu->addQueryAttribute("0020","000D","\0"); // studyInstanceUID
    m_findScu->addQueryAttribute("0020","0010","\0"); // study ID
   

    // we repeat the find process with the data from all selected nodes
    Node* selNode = selectedNodes->getFirst();
    while (selNode != NULL)
    {
        // do the work
        ConnData cdata = selNode->getConnData();
        m_findScu->sendFindRequest(cdata.title.c_str(),cdata.ip.c_str(),cdata.port,m_ourTitle.c_str(), m_ourIP.c_str(), m_ourPort);

        selNode = selectedNodes->getNext();
    }


    // now extract information from datasets and build a visual list
    QString text;
    int nodeCounter = 0;
    int itemCounter = 0;
    int sum = 0;
    ui->treeWidget->clear();
    dcmtkNodeContainer* resCont = m_findScu->getResultNodeContainer();
    Node* myNode = resCont->getFirst();
    while (myNode != NULL)
    {
        dcmtkFindDataset* myDs = myNode->getDatasetContainer()->getFirst();
        while (myDs != NULL)
        {
            // add result to list
            QTreeWidgetItem *pItem = new QTreeWidgetItem(ui->treeWidget);
            pItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
            QPoint refPoint(nodeCounter,itemCounter);
            QVariant vr = refPoint;
            pItem->setData(0,Qt::UserRole, vr);
            text = myDs->m_patientName.c_str();
            pItem->setText(0,text);
            ui->treeWidget->insertTopLevelItem(0,pItem);
            myDs = myNode->getDatasetContainer()->getNext();
            itemCounter++;
            sum++;
        }
        myNode = resCont->getNext();
        nodeCounter++;
        itemCounter=0;
    }

    // print to logwindow
    printResults(sum,"studies");

    delete selectedNodes;
}

//---------------------------------------------------------------------------------------------

void SimpleView::findSeriesLevel(QTreeWidgetItem * item)
{
    // check if its a top level item
    if (item->parent() != NULL) 
    {
        findImageLevel(item);
        return;
    }

    // check if the item alread has children (don't query again)
    if (item->child(0) != NULL) return;

    dcmtkNodeContainer* resCont =  m_findScu->getResultNodeContainer();

    QPoint pt = item->data(0,Qt::UserRole).toPoint();
    dcmtkFindDataset* resDs = resCont->getAtPos(pt.x())->getDatasetContainer()->getAtPos(pt.y());

    std::string studInst = resDs->m_studyInstUID;
    ConnData cdata = resCont->getAtPos(pt.x())->getConnData();

    // clear previous results
    dcmtkFindScu* findScu = new dcmtkFindScu;

    // set up search criteria
    findScu->setQueryLevel(dcmtkFindScu::SERIES);
    findScu->addQueryAttribute("0020","000D",studInst.c_str()); // studyInstanceUID
    findScu->addQueryAttribute("0008","0021","\0"); // series date
    findScu->addQueryAttribute("0008","0031","\0"); // series time
    findScu->addQueryAttribute("0008","0060","\0"); // series modality
    findScu->addQueryAttribute("0008","103E","\0"); // series description
    findScu->addQueryAttribute("0018","0015","\0"); // body part
    findScu->addQueryAttribute("0018","1030","\0"); // protocol name
    findScu->addQueryAttribute("0018","5100","\0"); // patient position
    findScu->addQueryAttribute("0020","000E","\0"); // series instance UID
    findScu->addQueryAttribute("0020","0011","\0"); // series number
    findScu->addQueryAttribute("0020","0052","\0"); // frame of reference



    // do the work
    findScu->sendFindRequest(cdata.title.c_str(),cdata.ip.c_str(),cdata.port,m_ourTitle.c_str(), m_ourIP.c_str(), m_ourPort);

    // now extract information from datasets and build a visual list
    QString text;
    int nodeCounter = 0;
    int itemCounter = 0;
    int sum = 0;
    resCont = findScu->getResultNodeContainer();
    Node* myNode = resCont->getFirst();
    while (myNode != NULL)
    {
        dcmtkFindDataset* myDs = myNode->getDatasetContainer()->getFirst();
        while (myDs != NULL)
        {
            // add result to list
            QTreeWidgetItem *pItem = new QTreeWidgetItem(item);
            pItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
            QPoint refPoint(nodeCounter,itemCounter);
            QVariant vr = refPoint;
            pItem->setData(0,Qt::UserRole, vr);
            QString refString(myDs->m_seriesInstUID.c_str());
            pItem->setData(1,Qt::UserRole, refString);
            text = myDs->m_seriesDescr.c_str();
            pItem->setText(0,text);
            item->addChild(pItem);
            myDs = myNode->getDatasetContainer()->getNext();
            itemCounter++;
            sum++;
        }
        myNode = resCont->getNext();
        nodeCounter++;
        itemCounter=0;
    }

    // print to logwindow
    printResults(sum,"series");

    // clean up
    delete findScu;
}

//---------------------------------------------------------------------------------------------

void SimpleView::findImageLevel(QTreeWidgetItem * item)
{
    // check if the item alread has children (don't query again)
    if (item->child(0) != NULL) return;

    dcmtkNodeContainer* resCont =  m_findScu->getResultNodeContainer();

    QPoint pt = item->data(0,Qt::UserRole).toPoint();
    dcmtkFindDataset* resDs = resCont->getAtPos(pt.x())->getDatasetContainer()->getAtPos(pt.y());

    QString refString = item->data(1,Qt::UserRole).toString();

    std::string serInst = resDs->m_studyInstUID;
    ConnData cdata = resCont->getAtPos(pt.x())->getConnData();

    // clear previous results
    dcmtkFindScu* findScu = new dcmtkFindScu;

    // set up search criteria
    findScu->setQueryLevel(dcmtkFindScu::IMAGE);
    findScu->addQueryAttribute("0020","000E",refString.toLatin1()); // series instance UID
    findScu->addQueryAttribute("0008","0008","\0"); // image type
    findScu->addQueryAttribute("0008","0012","\0"); // instance creation date
    findScu->addQueryAttribute("0008","0013","\0"); // instance creation time
    findScu->addQueryAttribute("0008","0016","\0"); // SOP class UID
    findScu->addQueryAttribute("0008","0018","\0"); // SOP instance UID
    findScu->addQueryAttribute("0008","0022","\0"); // image date
    findScu->addQueryAttribute("0008","0032","\0"); // image time
    findScu->addQueryAttribute("0020","0012","\0"); // aquisition number
    findScu->addQueryAttribute("0020","000D","\0"); // study instance UID
    findScu->addQueryAttribute("0020","0013","\0"); // instance time
    findScu->addQueryAttribute("0020","0032","\0"); // image position patient
    findScu->addQueryAttribute("0020","0037","\0"); // image orientation patient
    findScu->addQueryAttribute("0020","1041","\0"); // slice location
    findScu->addQueryAttribute("0028","0008","\0"); // number of frames
    
    // do the work
    
    findScu->sendFindRequest(cdata.title.c_str(),cdata.ip.c_str(),cdata.port,m_ourTitle.c_str(), m_ourIP.c_str(), m_ourPort);

    // now extract information from datasets and build a visual list
    QString text;
    int nodeCounter = 0;
    int itemCounter = 0;
    int sum = 0;
    resCont = findScu->getResultNodeContainer();
    Node* myNode = resCont->getFirst();
    while (myNode != NULL)
    {
        dcmtkFindDataset* myDs = myNode->getDatasetContainer()->getFirst();
        while (myDs != NULL)
        {
            // add result to list
            QTreeWidgetItem *pItem = new QTreeWidgetItem(item);
            QPoint refPoint(nodeCounter,itemCounter);
            QVariant vr = refPoint;
            pItem->setData(1,Qt::UserRole, vr);
            text = myDs->m_imageType.c_str();
            pItem->setText(0,text);
            item->addChild(pItem);
            myDs = myNode->getDatasetContainer()->getNext();
            itemCounter++;
            sum++;
        }
        myNode = resCont->getNext();
        nodeCounter++;
        itemCounter=0;
    }

    // print to logwindow
    printResults(sum,"images");

    // clean up
    delete findScu;
}

//---------------------------------------------------------------------------------------------

void SimpleView::move(QTreeWidgetItem * item, int column) 
{
   dcmtkNodeContainer* resCont =  m_findScu->getResultNodeContainer();

   QPoint pt = item->data(0,Qt::UserRole).toPoint();
   dcmtkFindDataset* resDs = resCont->getAtPos(pt.x())->getDatasetContainer()->getAtPos(pt.y());

   
    ui->logWindow->append(tr("Fetching data..."));
    QString patString = "Selected patient: ";
    patString.append(resDs->m_patientName.c_str());
    ui->logWindow->append(patString);
    ui->logWindow->repaint();

    // set up search criteria
    m_moveScu->addQueryAttribute("0020", "000D", resDs->m_studyInstUID.c_str());
    
    // send the move request using the search crits
    ConnData cdata = resCont->getAtPos(pt.x())->getConnData();
    m_moveScu->sendMoveRequest(cdata.title.c_str(),cdata.ip.c_str(),cdata.port,m_ourTitle.c_str(), m_ourIP.c_str(), m_ourPort);
    
    ui->logWindow->append(tr("All done."));

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
    ConnData* cb = &(m_nodes.at(ui->sendToNodeCB->currentIndex()));
    m_sendThread->setConnectionParams(cb->title.c_str(), cb->ip.c_str(), cb->port, m_ourTitle.c_str(), m_ourIP.c_str(), m_ourPort);
    
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

    std::vector<ConnData>::iterator iter;
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
        // remove all items comming after the last one
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
    
    ui->peerTitleEdit->setText(peerTitle);
    ui->peerIPEdit->setText(peerIP);
    ui->peerPortEdit->setText(peerPort);

    // apply only if none-empty
    if(!ourTitle.isEmpty()) ui->ourTitleEdit->setText(ourTitle);
    if(!ourIP.isEmpty()) ui->ourIPEdit->setText(ourIP);
    if(!ourPort.isEmpty()) ui->ourPortEdit->setText(ourPort);

}

//---------------------------------------------------------------------------------------------

void SimpleView::fileAppender(int state)
{
 switch(state)
 {
 case 0:
      Logger::removeOutput(m_fileOut);
      break;
 case 2:
      Logger::addOutput(m_fileOut);
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
      Logger::removeOutput(m_shellOut);
      break;
 case 2:
      Logger::addOutput(m_shellOut);
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
      Logger::removeOutput(m_widgetOut);
      break;
 case 2:
      Logger::addOutput(m_widgetOut);
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
    m_serverThread->terminate();
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

    ConnData connData;
    connData.title = peer.toStdString();
    connData.ip    = ip.toStdString();
    connData.port  = port.toInt();

    m_nodes.push_back(connData);

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

dcmtkNodeContainer* SimpleView::getSelectedNodes()
{
    //build selected node container
    dcmtkNodeContainer* selectedNodes = new dcmtkNodeContainer;
    for(int i=0; i < ui->nodeSelectionLW->count(); i++)
    {
      QListWidgetItem *item = ui->nodeSelectionLW->item(i);
      if( item->checkState() == Qt::Checked)
      {
          Node node;
          node.addConnData(m_nodes.at(i));
          selectedNodes->addNode(&node);
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

