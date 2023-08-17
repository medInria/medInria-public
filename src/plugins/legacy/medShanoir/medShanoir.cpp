#include "medShanoir.h"
#include <QObject>

#include <Levels.h>

ShanoirPlugin::ShanoirPlugin()
	:m_auth(m_net, m_settings), m_username("username"), m_password("password"), m_hostname("hostname"), m_rm(m_auth, m_net)
{
	m_password.setDefaultRepresentation(4);
	m_hostname.setValue("shanoir.irisa.fr"); // default value for new instances
}

ShanoirPlugin::~ShanoirPlugin()
{
}

/* ***********************************************************************/
/* *************** Init/set/ctrl source properties ***********************/
/* ***********************************************************************/
bool ShanoirPlugin::initialization(QString const  &pi_instanceId)
{
	bool instance_id_exists = !pi_instanceId.isEmpty();
	if(instance_id_exists)
	{
		m_instance_id = pi_instanceId;
		m_settings.setInstanceName(m_instance_id);
		m_auth.retrieveToken();
	}
	return instance_id_exists;
}

bool ShanoirPlugin::setInstanceName(QString const &pi_instanceName)
{
	bool instance_name_exists = !pi_instanceName.isEmpty(); 
	if(instance_name_exists)
	{
		m_instance_name = pi_instanceName;
	}
	return instance_name_exists;
}



bool ShanoirPlugin::connect(bool pi_bEnable)
{
	if(pi_bEnable)
	{
		if(m_auth.isAuthenticated())
		{ // THE TOKEN IS STILL EFFECTIVE : A REFRESH IS SUFFICIENT
			m_auth.autoRefreshAccessToken();
		}
		else
		{ // THE TOKEN EXPIRED OR IT IS THE FIRST TIME WE CONNECT
			QString username = m_username.value();
			QString password = m_password.value();
			QString domain = m_hostname.value();
			m_auth.initAuthentication(domain, username,password);
		}
	}
	else 
	{
		m_auth.disauthenticate();
	}
	emit connectionStatus(isOnline());
	return  pi_bEnable==isOnline();
}

QList<medAbstractParameter*> ShanoirPlugin::getAllParameters()
{
	return  { &m_hostname, &m_username, &m_password};
}

QList<medAbstractParameter*> ShanoirPlugin::getCipherParameters() 
{
	return {&m_password};
}

QList<medAbstractParameter*> ShanoirPlugin::getVolatilParameters() 
{
	return {};
}

QList<medAbstractParameter*> ShanoirPlugin::getFilteringParameters()
{
	return {};
}

/* ***********************************************************************/
/* *************** Get source properties *********************************/
/* ***********************************************************************/
bool ShanoirPlugin::isWritable()
{
	return false;
}

bool ShanoirPlugin::isLocal()
{
	return false;
}

bool ShanoirPlugin::isCached()
{
	return false;
}

bool ShanoirPlugin::isOnline()
{
	return m_auth.isAuthenticated();
}

bool ShanoirPlugin::isFetchByMinimalEntriesOrMandatoryAttributes()
{
	return true;
}


int ShanoirPlugin::getIOInterface()
{
	return IO_FILE;
}


QMap<QString, QStringList> ShanoirPlugin::getTypeAndFormat()
{
	QMap<QString, QStringList> map;
	return map;
}


/* ***********************************************************************/
/* *************** Get source structure information **********************/
/* ***********************************************************************/
QString          ShanoirPlugin::getInstanceName()
{
	return m_instance_name;
}

QString          ShanoirPlugin::getInstanceId()
{
	return m_instance_id;
}

unsigned int     ShanoirPlugin::getLevelCount()
{
	return s_level_names.size();
}

unsigned int    ShanoirPlugin::getLevelDesiredWritable()
{
	// 6 is the index level of the processed datasets.
	// it is this type of dataset that shanoir allows to create
	// (if we have the necessary rights)
	return 6; 
}

QStringList      ShanoirPlugin::getLevelNames()
{
	return s_level_names;
}

QString          ShanoirPlugin::getLevelName(unsigned int pi_uiLevel)
{
	return s_level_names[pi_uiLevel];
}

bool             ShanoirPlugin::isLevelWritable(unsigned int pi_uiLevel) {
	return false;
}

QStringList      ShanoirPlugin::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
{
	if(pi_uiLevel < getLevelCount())
	{
		return { "key", "name", "description" };
	}
	else
	{
		return {};
	}
}


/* ***********************************************************************/
/* *************** Get elements data *************************************/
/* ***********************************************************************/

QList<medAbstractSource::levelMinimalEntries>    ShanoirPlugin::getMinimalEntries(unsigned int pi_uiLevel, QString parentKey)
{
	QList<medAbstractSource::levelMinimalEntries> entries;
	switch (pi_uiLevel)
	{
	case 0:
		entries = getRootMinimalEntries();
		break;
	case 1:
		entries = getStudyMinimalEntries(parentKey);
		break;
	case 2:
		entries = getSubjectMinimalEntries(parentKey);
		break;
	case 3:
		entries = getExaminationMinimalEntries(parentKey);
		break;
	case 4:
		entries = getDatasetAcquisitionMinimalEntries(parentKey);
		break;
	case 5:
		entries = getDatasetMinimalEntries(parentKey);
		break;
	case 6:
		entries = getProcessingDatasetMinimalEntries(parentKey);
		break;
	default:
		qDebug()<<pi_uiLevel<<" is not an accurate level";
		break;
	}
    return entries;
}

QList<medAbstractSource::levelMinimalEntries>  ShanoirPlugin::getRootMinimalEntries()
{
	QList<levelMinimalEntries> entries;
	QList<StudyOverview> studies =  m_rm.getStudies();
	for(StudyOverview study : studies)
	{
		QString description = study.name;
		entries.append({ QString::number(study.id), study.name, description, entryType::folder});
	}
	return entries;
}

QList<medAbstractSource::levelMinimalEntries>  ShanoirPlugin::getStudyMinimalEntries(QString id)
{
	QList<levelMinimalEntries> entries;
	Study study = m_rm.getStudyById(id.toInt());
	for(SubjectOverview subject : study.subjects)
	{
		QString key =  QString::number(study.id)+"."+ QString::number(subject.id);
		QString description = subject.name + (subject.type.size() > 0 ? (" (" + subject.type + ")") : "");
		entries.append({key, subject.name,  description, entryType::folder });
	}
	return entries;
}
QList<medAbstractSource::levelMinimalEntries>  ShanoirPlugin::getSubjectMinimalEntries(QString parentkey)
{
	QList<levelMinimalEntries> entries;
	QStringList parts = parentkey.split('.');
	if(parts.size()==2)
	{
		int id_study = parts[0].toInt();
		int id_subject = parts[1].toInt();
		QList<Examination> examinations = m_rm.getExaminationsByStudySubjectId(id_study,id_subject);
		for(Examination examination : examinations)
		{
			QString key =parentkey+"."+QString::number(examination.id);
			QString name = examination.date.toString("dd/MM/yyyy") +", "+  examination.comment +"(id="+ QString::number(examination.id)+")";
			QString description = "Contains " +QString::number(examination.ds_acquisitions.size())+" dataset-acquisitions"; 
			entries.append({key, name, description, entryType::folder});
		}
	}
	return entries;
}


QList<medAbstractSource::levelMinimalEntries>  ShanoirPlugin::getExaminationMinimalEntries(QString parentkey)
{
	QList<levelMinimalEntries> entries;
	QStringList parts = parentkey.split('.');
	if(parts.size()==3) 
	{
		int id_study = parts[0].toInt();
		int id_subject = parts[1].toInt();
		int id_exam = parts[2].toInt();
		QList<Examination> examinations = m_rm.getExaminationsByStudySubjectId(id_study, id_subject);
		auto exam_it = findLevelElement(examinations,id_exam);
		if(exam_it!=examinations.end())
		{
			Examination exam = *exam_it;
			QList<DatasetAcquisition> ds_acq = exam.ds_acquisitions;
			for(DatasetAcquisition acquisition  : ds_acq)
			{
				QString key = parentkey+"."+ QString::number(acquisition.id);
				QString description = "Contains "+QString::number(acquisition.datasets.size())+" datasets";
				entries.append({key, acquisition.name, description, entryType::folder});
			}
		}
	}
	return entries;
}

QList<medAbstractSource::levelMinimalEntries>  ShanoirPlugin::getDatasetAcquisitionMinimalEntries(QString parentkey)
{
	QList<levelMinimalEntries> entries;
	QStringList parts = parentkey.split('.');
	if (parts.size() == 4)
	{
		int id_study = parts[0].toInt();
		int id_subject = parts[1].toInt();
		int id_exam = parts[2].toInt();
		int id_acq = parts[3].toInt();
		QList<Examination> examinations = m_rm.getExaminationsByStudySubjectId(id_study, id_subject);
		auto exam_it = findLevelElement(examinations, id_exam);
		if (exam_it != examinations.end())
		{
			Examination exam = *exam_it;
			QList<DatasetAcquisition> ds_acquisitions = exam.ds_acquisitions;
			auto dsacq_it = findLevelElement(ds_acquisitions, id_acq);
			if (dsacq_it != ds_acquisitions.end())
			{
				DatasetAcquisition ds_acq = *dsacq_it;
				QList<Dataset> datasets = ds_acq.datasets;
				for (Dataset ds : datasets)
				{
					QString key = parentkey + "." + QString::number(ds.id);
					QString description = "Dataset n°" + QString::number(ds.id) + " of type " + ds.type + " with " + QString::number(ds.processings.size()) + " processings";
					entries.append({ key, ds.name, description, entryType::both });
				}
			}
		}
	}
	return entries;
}

QList<medAbstractSource::levelMinimalEntries> ShanoirPlugin::getDatasetMinimalEntries(QString parentkey)
{
	QList<levelMinimalEntries> entries;
	QStringList parts = parentkey.split('.');
	if (parts.size() == 5)
	{
		int id_study = parts[0].toInt();
		int id_subject = parts[1].toInt();
		int id_exam = parts[2].toInt();
		int id_acq = parts[3].toInt();
		int id_ds = parts[4].toInt();
		QList<Examination> examinations = m_rm.getExaminationsByStudySubjectId(id_study, id_subject);
		auto exam_it = findLevelElement(examinations, id_exam);
		if (exam_it != examinations.end())
		{
			Examination exam = *exam_it;
			QList<DatasetAcquisition> ds_acquisitions = exam.ds_acquisitions;
			auto dsacq_it = findLevelElement(ds_acquisitions, id_acq);
			if (dsacq_it != ds_acquisitions.end())
			{
				DatasetAcquisition ds_acq = *dsacq_it;
				QList<Dataset> datasets = ds_acq.datasets;
				auto ds_it = findLevelElement(datasets, id_ds);
				if (ds_it != datasets.end())
				{
					Dataset ds = *ds_it;
					QList<DatasetProcessing> processings = ds.processings;
					for (auto processing : processings)
					{
						QString key = parentkey + "." + QString::number(processing.id);
						QString description = "dataset processing with " + QString::number(processing.inputDatasets.size()) + " input datasets and " + QString::number(processing.outputDatasets.size()) + " output datasets";
						entries.append({ key, processing.type, description, entryType::folder });
					}
				}
			}
		}
	}
	return entries;
}


QList<medAbstractSource::levelMinimalEntries> ShanoirPlugin::getProcessingDatasetMinimalEntries(QString parentkey)
{
	QList<levelMinimalEntries> entries;
	QStringList parts = parentkey.split('.');
	if(parts.size()==6)
	{
		int id_study = parts[0].toInt();
		int id_subject = parts[1].toInt();
		int id_exam = parts[2].toInt();
		int id_acq = parts[3].toInt();
		int id_ds = parts[4].toInt();
		int id_processing = parts[5].toInt();
		QList<Examination> examinations = m_rm.getExaminationsByStudySubjectId(id_study, id_subject);
		auto exam_it = findLevelElement(examinations,id_exam);
		if(exam_it!=examinations.end())
		{
			Examination exam = *exam_it;
			QList<DatasetAcquisition> ds_acquisitions = exam.ds_acquisitions;
			auto dsacq_it = findLevelElement(ds_acquisitions,id_acq);
			if(dsacq_it!=ds_acquisitions.end())
			{
				DatasetAcquisition ds_acq = *dsacq_it;
				QList<Dataset> datasets = ds_acq.datasets;
				auto ds_it = findLevelElement(datasets,id_ds);
				if(ds_it!=datasets.end())
				{
					Dataset ds = *ds_it;
					QList<DatasetProcessing> processings = ds.processings;
					auto processing_it = findLevelElement(processings,id_processing);
					if(processing_it!=processings.end())
					{
						DatasetProcessing processing = *processing_it;
						QList<ProcessedDataset> outputDatasets = processing.outputDatasets;
						for(auto outputDataset : outputDatasets)
						{
							QString key = parentkey + "." + QString::number(outputDataset.id);
							QString description = "processed dataset of type " + outputDataset.type;
							entries.append({key, outputDataset.name, description, entryType::dataset});
						}
					}
				}
			}
		}
	}
	return entries;
}


QList<QMap<QString, QString>> ShanoirPlugin::entriesToMandatoryAttributes(const QList<medAbstractSource::levelMinimalEntries>  &entries)
{
	QList<QMap<QString, QString>> mandatories;
	for(const levelMinimalEntries &entry : entries)
	{
		QString type = entry.type==entryType::folder?"folder":"dataset";
		QMap<QString, QString> mandatory ={{"key",entry.key}, {"name", entry.name}, {"description" , entry.description}, {"type", type}};
		mandatories.append(mandatory);
	}
	return mandatories;
}

QList<QMap<QString, QString>> ShanoirPlugin::getMandatoryAttributes(unsigned int pi_uiLevel, QString parentKey)
{
	return entriesToMandatoryAttributes(getMinimalEntries(pi_uiLevel,parentKey));
}
bool  ShanoirPlugin::getAdditionalAttributes(unsigned int pi_uiLevel, QString key, datasetAttributes &po_attributes)
{
	return false;
}


/* ***********************************************************************/
/* *************** Get data          *************************************/
/* ***********************************************************************/
QVariant ShanoirPlugin::getDirectData(unsigned int pi_uiLevel, QString parentKey)
{
	QVariant variant;
	QStringList parts = parentKey.split('.');
	bool dataset_level = pi_uiLevel == 4 && parts.size() == 5;
	bool ps_dataset_level = pi_uiLevel == 6 && parts.size() == 7;
	if(dataset_level || ps_dataset_level) // dataset level or processed dataset
	{
		int id_ds = parts[4].toInt();
		if (ps_dataset_level)
		{
			id_ds = parts[6].toInt();
		}
		//Le path renvoyé est le path d'un dossier contenant le nifiti. Cela fonctionne dans ce cadre mais il faut avoir en tête que ce n'est pas un path de fichier
		QString path = m_rm.loadNiftiDataset(id_ds); //m_rm.loadDicomDataset(id_ds); 
		variant = path;
	}
	return variant;
}

int      ShanoirPlugin::getAssyncData(unsigned int pi_uiLevel, QString parentKey)
{
	int idRequest = -1;
	QStringList parts = parentKey.split('.');
	bool dataset_level = pi_uiLevel == 4 && parts.size() == 5;
	bool ps_dataset_level = pi_uiLevel == 6 && parts.size() == 7;
	if (dataset_level || ps_dataset_level) // dataset level or processed dataset
	{
		int id_ds = parts[4].toInt();
		if (ps_dataset_level)
		{
			id_ds = parts[6].toInt();
		}
		idRequest = m_rm.loadAsyncNiftiDataset(id_ds); 
	}
	return idRequest;
}

/* ***********************************************************************/
/* *************** Store data          ***********************************/
/* ***********************************************************************/
bool ShanoirPlugin::addDirectData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{
	return true;
}

int  ShanoirPlugin::addAssyncData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{
	return 0;
}

bool ShanoirPlugin::createPath(QList<levelMinimalEntries> &pio_path, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString parentKey)
{
	return false;
}

bool ShanoirPlugin::createFolder(levelMinimalEntries &pio_minimalEntries, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString parentKey)
{
	return false;
}


bool ShanoirPlugin::alterMetaData(datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString key)
{
	return false;
}

bool ShanoirPlugin::getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key)
{
	return false;
}

bool ShanoirPlugin::setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key)
{
	return false;
}

bool ShanoirPlugin::commitData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{
	return false;
}

int  ShanoirPlugin::push(unsigned int pi_uiLevel, QString key)
{
	return 0;
}

/* ***********************************************************************/
/* *************** Handle async results   ********************************/
/* ***********************************************************************/
QVariant ShanoirPlugin::getAsyncResults(int pi_iRequest)
{
	QVariant variant;
	QString path = m_rm.getAsyncResult(pi_iRequest);
	variant = path;
	return variant;
}

void ShanoirPlugin::dataReception(int idRequest)
{
	emit progress(idRequest, finish);
}

/* ***********************************************************************/
/* *************** Optional functions  ***********************************/
/* ***********************************************************************/


void ShanoirPlugin::abort(int pi_iRequest){}