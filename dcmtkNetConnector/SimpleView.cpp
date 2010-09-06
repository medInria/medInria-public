#include "ui_SimpleView.h"
#include "SimpleView.h"
#include <QSettings>

#include "dcmtkEchoScu.h"
#include "dcmtkFindScu.h"
#include "dcmtkMoveScu.h"
#include "dcmtkStoreScu.h"
#include "dcmtkStoreScp.h"
#include "dcmtkFindDataset.h"

#include "Logger.h"
#include "LoggerFileOutput.h"
#include "LoggerConsoleOutput.h"

//---------------------------------------------------------------------------------------------

// Constructor
SimpleView::SimpleView() 
{
  //initialize logger
  Logger::startUp();
  Logger::addOutput(new LoggerFileOutput());
  //Logger::addOutput(new LoggerConsoleOutput());

  this->ui = new Ui_SimpleView;
  this->ui->setupUi(this);

  m_echoScu = new dcmtkEchoScu();
  m_findScu = new dcmtkFindScu();
  m_moveScu = new dcmtkMoveScu();
  m_storeScu = new dcmtkStoreScu();

  setConnectionParams();

  connect(this->ui->searchField, SIGNAL(returnPressed()), this, SLOT(search()));
  connect(this->ui->treeWidget, SIGNAL(itemDoubleClicked ( QTreeWidgetItem* , int )), this, SLOT(move(QTreeWidgetItem *, int)));
  connect(this->ui->echoButton, SIGNAL(clicked()), this, SLOT(testConnection()));

  retrieveSettings();

};

//---------------------------------------------------------------------------------------------

void SimpleView::setConnectionParams()
{

    m_ourTitle = "DCMTKTOOL";
    m_ourIP    = "localhost";
    m_ourPort  = 100;
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
     
    for (iter= m_resultSet.begin(); iter != m_resultSet.end(); iter++)
    {
        // add result to list
        QTreeWidgetItem *pItem = new QTreeWidgetItem(ui->treeWidget);
        pItem->setText(0,(*iter)->m_patientName.c_str());
        ui->treeWidget->insertTopLevelItem(0,pItem);
    }

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

        storeSettings();
    }
    else
        ui->logWindow->append(tr("No response from peer. Check your connection"));

}

//---------------------------------------------------------------------------------------------

void SimpleView::storeSettings()
{
    QSettings settings("INRIA", "DCMTKTOOL");
    settings.beginGroup("Connection");
    settings.setValue("PEER_AET", ui->peerTitleEdit->text());
    settings.setValue("PEER_IP", ui->peerIPEdit->text());
    settings.setValue("PEER_PORT", ui->peerPortEdit->text());
    settings.endGroup();
}

//---------------------------------------------------------------------------------------------

void SimpleView::retrieveSettings()
{
    QString peerTitle, peerIP, peerPort;
    QSettings settings("INRIA", "DCMTKTOOL");
    settings.beginGroup("Connection");
    peerTitle = settings.value("PEER_AET").value<QString>();
    peerIP = settings.value("PEER_IP").value<QString>();
    peerPort = settings.value("PEER_PORT").value<QString>();
    settings.endGroup();
    
    ui->peerTitleEdit->setText(peerTitle);
    ui->peerIPEdit->setText(peerIP);
    ui->peerPortEdit->setText(peerPort);

}

//---------------------------------------------------------------------------------------------
