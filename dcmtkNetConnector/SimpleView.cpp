#include "ui_SimpleView.h"
#include "SimpleView.h"
#include <QSettings>
#include <QDockWidget>

#include "dcmtkEchoScu.h"
#include "dcmtkFindScu.h"
#include "dcmtkMoveScu.h"
#include "dcmtkStoreScu.h"
#include "dcmtkStoreScp.h"
#include "dcmtkFindDataset.h"

#include "Logger.h"
#include "LoggerFileOutput.h"
#include "LoggerConsoleOutput.h"
#include "LoggerWidgetOutput.h"

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
  m_storeScu = new dcmtkStoreScu();

  m_shellOut = new LoggerConsoleOutput();
  m_fileOut = new LoggerFileOutput();
  m_widgetOut = new LoggerWidgetOutput();
  
  setConnectionParams();

  connect(this->ui->searchField, SIGNAL(returnPressed()), this, SLOT(search()));
  connect(this->ui->treeWidget, SIGNAL(itemDoubleClicked ( QTreeWidgetItem* , int )), this, SLOT(move(QTreeWidgetItem *, int)));
  connect(this->ui->echoButton, SIGNAL(clicked()), this, SLOT(testConnection()));
  connect(this->ui->applySettingsButton, SIGNAL(clicked()), this, SLOT(applySettingsSlot()));
  connect(this->ui->cbFileTarget, SIGNAL(stateChanged(int)), this, SLOT(fileAppender(int)));
  connect(this->ui->cbShellTarget, SIGNAL(stateChanged(int)), this, SLOT(shellAppender(int)));
  connect(this->ui->cbWidgetTarget, SIGNAL(stateChanged(int)), this, SLOT(widgetAppender(int)));
  connect(this->ui->cbLogLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLogLevel(int)));
  
  retrieveSettings();

  //set logger defaults
  ui->cbWidgetTarget->setChecked(true);
  ui->cbLogLevel->setCurrentIndex(2);

  // create and show LoggerWidget
  QDockWidget* loggerDock = new QDockWidget(this);
  loggerDock->setWidget(m_widgetOut);
  this->addDockWidget(Qt::BottomDockWidgetArea, loggerDock);

};

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


    m_findScu->setConnectionParams(m_peerTitle.c_str(), m_peerIP.c_str(), m_peerPort, m_ourTitle.c_str(), m_ourIP.c_str(), m_ourPort);
    m_moveScu->setConnectionParams(m_peerTitle.c_str(), m_peerIP.c_str(), m_peerPort, m_ourTitle.c_str(), m_ourIP.c_str(), m_ourPort);
    m_echoScu->setConnectionParams(m_peerTitle.c_str(), m_peerIP.c_str(), m_peerPort, m_ourTitle.c_str(), m_ourIP.c_str(), m_ourPort);
    m_storeScu->setConnectionParams(m_peerTitle.c_str(), m_peerIP.c_str(), m_peerPort, m_ourTitle.c_str(), m_ourIP.c_str(), m_ourPort);
}

//---------------------------------------------------------------------------------------------

SimpleView::~SimpleView()
{
  delete m_findScu;
  delete m_moveScu;
  delete m_echoScu;
  delete m_storeScu;

  delete m_shellOut;
  delete m_fileOut;
  delete m_widgetOut;
}

//---------------------------------------------------------------------------------------------

// Action to be taken upon file open 
void SimpleView::search()
{
    setConnectionParams();

    QString patientName = ui->searchField->text();
    patientName.append("*");

    // set up search criteria
    m_findScu->clearAllQueryAttributes();
    m_findScu->setQueryLevel(dcmtkFindScu::STUDY);
    m_findScu->addQueryAttribute("0010", "0010", patientName.toLatin1()); // search for patient
    m_findScu->addQueryAttribute("0020", "000D", "\0"); // studyInstanceUID
    m_findScu->addQueryAttribute("0020","0010","\0"); // study ID
    m_findScu->addQueryAttribute("0008","0030","\0"); // study date
    m_findScu->addQueryAttribute("0008","0050","\0"); // accession no
    m_findScu->addQueryAttribute("0008","0090","\0"); // ref physician
    m_findScu->addQueryAttribute("0008","1030","\0"); // study description
    m_findScu->addQueryAttribute("0010","0020","\0"); // patient ID
    m_findScu->addQueryAttribute("0010","0030","\0"); // patient BD
    m_findScu->addQueryAttribute("0010","0040","\0"); // sex
    m_findScu->addQueryAttribute("0008","0061","\0"); // modalities in study
    
    // do the work
    m_findScu->sendFindRequest();

    m_resultSet = m_findScu->getResultDataset();
    ui->treeWidget->clear();
    QString text;
    for (iter= m_resultSet.begin(); iter != m_resultSet.end(); iter++)
    {
        // add result to list
        QTreeWidgetItem *pItem = new QTreeWidgetItem(ui->treeWidget);
        text = (*iter)->m_patientName.c_str();
        pItem->setText(0,text);
        ui->treeWidget->insertTopLevelItem(0,pItem);
    }
    text.clear();
    text.setNum(m_resultSet.size());
    text.append(" patient(s) found");
    ui->logWindow->append(text);

}

//---------------------------------------------------------------------------------------------

void SimpleView::move(QTreeWidgetItem * item, int column) 
{
    m_resultSet = m_findScu->getResultDataset();

    int index = ui->treeWidget->indexOfTopLevelItem(item);
    ui->logWindow->append(tr("Fetching data..."));
    QString patString = "Selected patient: ";
    patString.append(m_resultSet.at(index)->m_patientName.c_str());
    ui->logWindow->append(patString);
    ui->logWindow->repaint();

    // set up search criteria
    m_moveScu->addQueryAttribute("0020", "000D", m_resultSet.at(index)->m_studyInstUID.c_str());
    m_moveScu->useBuildInStoreSCP(true);        
    
    // send the move request using the search crits
    m_moveScu->sendMoveRequest();
    m_moveScu->clearAllQueryAttributes();

    ui->logWindow->append(tr("All done."));

}

//---------------------------------------------------------------------------------------------

void SimpleView::testConnection()
{
    setConnectionParams();

    // now send the echo
    if ( m_echoScu->sendEchoRequest() == 0 )
    {
        ui->logWindow->append(tr("Connection verified successfully"));

        storeSettings(0);
    }
    else
        ui->logWindow->append(tr("No response from peer. Check your connection"));

}

//---------------------------------------------------------------------------------------------

void SimpleView::applySettingsSlot()
{
    storeSettings(1);
}

//---------------------------------------------------------------------------------------------

void SimpleView::storeSettings(int type)
{
    QSettings settings("INRIA", "DCMTKTOOL");

    switch(type)
    {
    case 0:
        settings.beginGroup("Connection");
        settings.setValue("PEER_AET", ui->peerTitleEdit->text());
        settings.setValue("PEER_IP", ui->peerIPEdit->text());
        settings.setValue("PEER_PORT", ui->peerPortEdit->text());
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
    QSettings settings("INRIA", "DCMTKTOOL");
    settings.beginGroup("Connection");
    peerTitle = settings.value("PEER_AET").value<QString>();
    peerIP = settings.value("PEER_IP").value<QString>();
    peerPort = settings.value("PEER_PORT").value<QString>();
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