//
// Created by Julien Castelneau on 11/10/2021.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include <PluginAPHP/QtDcmAPHP.h>
#include <sphereDicomWeb/medAnnotation.h>
#include "medAPHP.h"
#include "sphereDicomWeb/medAbstractAnnotation.h"


class FakeQtDcmAPHP : public QtDcmInterface
{
public:
    MOCK_METHOD(int, sendEcho, ());
    using entries = QList<QMap<QString, QString>> ;
    using filtersMap = QMap<QString, QString>;
    MOCK_METHOD(entries, findPatientMinimalEntries, (const filtersMap &));
    MOCK_METHOD(entries, findStudyMinimalEntries, (const QString &, const filtersMap &) );
    MOCK_METHOD(entries, findSeriesMinimalEntries, (const QString &, const filtersMap &) );
//    MOCK_METHOD(int, moveRequest, (const QString &, const QString &));
    MOCK_METHOD(void, stopMove, (int requestId));
    MOCK_METHOD(bool, moveRequest, (int, const QString &, const QString &));
    MOCK_METHOD(bool, isCachedDataPath, (int));
    MOCK_METHOD(void, updateLocalParameters, (QString const &, QString const &, int));
    MOCK_METHOD(void, updateRemoteParameters, (QString const &, QString const &, int));
};

class FakeAnnotation : public medAbstractAnnotation
{
public:
    MOCK_METHOD(int, isServerAvailable, ());
    using entries = QList<QMap<QString, QString>> ;
    MOCK_METHOD(entries, findAnnotationMinimalEntries, (const QString &));
    MOCK_METHOD(bool, getAnnotationData, (int, const QString &));
    MOCK_METHOD(bool, isCachedDataPath, (int));
    MOCK_METHOD(QString, addData, (QVariant, QString, QString&));
    MOCK_METHOD(void, abortDownload, (int));
    MOCK_METHOD(void, updateUrl, (QString const &));
};

class medAPHPGTest : public ::testing::Test
{
protected:
    ::testing::NiceMock<FakeQtDcmAPHP> fakeQtDCM;
    ::testing::NiceMock<FakeAnnotation> fakeAnnotation;
    medAPHP *m_;

    medAPHPGTest()
    {
    }

    ~medAPHPGTest() {};

    void SetUp() override
    {
        //fakeQtDCM = new FakeQtDcmAPHP();
        //fakeAnnotation = new FakeAnnotation();
        m_ = new medAPHP(&fakeQtDCM, &fakeAnnotation);
    }

    void TearDown() override
    {
//        delete m_;
    }
};

// Test Initialization
TEST_F(medAPHPGTest, test_init_failed_if_instance_id_empty)
{
    // parameters initialization
    QString instanceId = "";
    // expectations
    EXPECT_EQ(false, m_->initialization(instanceId));
    EXPECT_EQ(m_->getInstanceId(), instanceId);
}


TEST_F(medAPHPGTest, test_init_success_if_instance_id_not_empty)
{
    // parameters initialization
    QString instanceId = "foo";
    // expectations

    EXPECT_EQ(true, m_->initialization(instanceId));
    EXPECT_EQ(m_->getInstanceId(), instanceId);
}

// Test Connection
TEST_F(medAPHPGTest, test_connect_failed_pacs_return_minus_1_annotation_return_0)
{
    ON_CALL(fakeQtDCM, sendEcho).WillByDefault(::testing::Return(-1));
    ON_CALL(fakeAnnotation, isServerAvailable).WillByDefault(::testing::Return(0));

    EXPECT_CALL(fakeQtDCM, sendEcho).Times(1);
    EXPECT_CALL(fakeAnnotation, isServerAvailable).Times(0);

    EXPECT_EQ(m_->connect(true), false);
    EXPECT_EQ(m_->isOnline(), false);
}

TEST_F(medAPHPGTest, test_connect_failed_pacs_return_0_annotation_return_minus_102)
{
    ON_CALL(fakeQtDCM, sendEcho).WillByDefault(::testing::Return(0));
    ON_CALL(fakeAnnotation, isServerAvailable).WillByDefault(::testing::Return(-102));

    EXPECT_CALL(fakeQtDCM, sendEcho).Times(1);
    EXPECT_CALL(fakeAnnotation, isServerAvailable).Times(1);

    EXPECT_EQ(m_->connect(true), false);
    EXPECT_EQ(m_->isOnline(), false);
}

TEST_F(medAPHPGTest, test_connect_failed_pacs_return_minus_1_annotation_return_minus_102)
{
    ON_CALL(fakeQtDCM, sendEcho).WillByDefault(::testing::Return(-1));
    ON_CALL(fakeAnnotation, isServerAvailable).WillByDefault(::testing::Return(-102));

    EXPECT_CALL(fakeQtDCM, sendEcho).Times(1);
    EXPECT_CALL(fakeAnnotation, isServerAvailable).Times(0);

    EXPECT_EQ(m_->connect(true), false);
    EXPECT_EQ(m_->isOnline(), false);
}

TEST_F(medAPHPGTest, test_connect_success_pacs_return_0_annotation_return_0)
{
    ON_CALL(fakeQtDCM, sendEcho).WillByDefault(::testing::Return(0));
    ON_CALL(fakeAnnotation, isServerAvailable).WillByDefault(::testing::Return(0));

    EXPECT_CALL(fakeQtDCM, sendEcho).Times(1);
    EXPECT_CALL(fakeAnnotation, isServerAvailable).Times(1);

    EXPECT_EQ(m_->connect(true), true);
    EXPECT_EQ(m_->isOnline(), true);
}


TEST_F(medAPHPGTest, test_disconnect_success)
{
    EXPECT_CALL(fakeQtDCM, sendEcho).Times(0);
    EXPECT_CALL(fakeAnnotation, isServerAvailable).Times(0);

    EXPECT_EQ(m_->connect(false), true);
    EXPECT_EQ(m_->isOnline(), false);
}

TEST_F(medAPHPGTest, is_writeable_true)
{
    EXPECT_EQ(true, m_->isWritable());
}

TEST_F(medAPHPGTest, is_local_false)
{
    EXPECT_EQ(false, m_->isLocal());
}

TEST_F(medAPHPGTest, is_cached_false)
{
    EXPECT_EQ(false, m_->isCached());
}

TEST_F(medAPHPGTest, set_instance_name_failed_param_empty)
{
    QString instanceName;
    EXPECT_EQ(false, m_->setInstanceName(instanceName));
    EXPECT_EQ("", m_->getInstanceName());
}

TEST_F(medAPHPGTest, set_instance_name_succes_param_not_empty)
{
    QString instanceName = "foo";
    EXPECT_EQ(true, m_->setInstanceName(instanceName));
    EXPECT_EQ(instanceName, m_->getInstanceName());
}

TEST_F(medAPHPGTest, level_count_success)
{
    EXPECT_EQ(4, m_->getLevelCount());
}


TEST_F(medAPHPGTest, test_levelnames_success)
{
    QStringList names;
    names << "PATIENTS" << "STUDY" << "SERIES" << "ANNOTATION";
    EXPECT_EQ(names, m_->getLevelNames());
}

TEST_F(medAPHPGTest, test_levelname_1st_level_success)
{
    int level = 0;
    EXPECT_EQ("PATIENTS", m_->getLevelName(level));
}

TEST_F(medAPHPGTest, test_levelname_2nd_level_success)
{
    int level = 1;
    EXPECT_EQ("STUDY", m_->getLevelName(level));
}

TEST_F(medAPHPGTest, test_levelname_3rd_level_success)
{
    int level = 2;
    EXPECT_EQ("SERIES", m_->getLevelName(level));
}

TEST_F(medAPHPGTest, test_levelname_4th_level_success)
{
    int level = 3;
    EXPECT_EQ("ANNOTATION", m_->getLevelName(level));
}

TEST_F(medAPHPGTest, test_levelname_negative_level_failed)
{
    int level = -1;
    EXPECT_EQ("Invalid Level Name", m_->getLevelName(level));
}

TEST_F(medAPHPGTest, test_levelname_invalid_level_failed)
{
    int level = 4;
    EXPECT_EQ("Invalid Level Name", m_->getLevelName(level));
}

TEST_F(medAPHPGTest, test_mandatory_attr_keys_1st_level_success)
{
    int level = 0;
    QStringList mandatory_keys;
    mandatory_keys << "id" << "description" << "patientID" << "gender" << "birthdate";
    EXPECT_EQ(mandatory_keys, m_->getMandatoryAttributesKeys(level));
}

TEST_F(medAPHPGTest, test_mandatory_attr_keys_2nd_level_success)
{
    int level = 1;
    QStringList mandatory_keys;
    mandatory_keys << "id" << "description" << "uid" << "study date";
    EXPECT_EQ(mandatory_keys, m_->getMandatoryAttributesKeys(level));
}

TEST_F(medAPHPGTest, test_mandatory_attr_keys_3rd_level_success)
{
    int level = 2;
    QStringList mandatory_keys;
    mandatory_keys << "id" << "description" << "uid" << "institution name" << "institution address" << "acquisition number" << "number of series related instances";
    EXPECT_EQ(mandatory_keys, m_->getMandatoryAttributesKeys(level));
}

TEST_F(medAPHPGTest, test_mandatory_attr_keys_4rd_level_success)
{
    int level = 3;
    QStringList mandatory_keys;
    mandatory_keys << "id" << "description" << "uid";
    EXPECT_EQ(mandatory_keys, m_->getMandatoryAttributesKeys(level));
}

TEST_F(medAPHPGTest, test_mandatory_attr_keys_negative_level_failed)
{
    int level = -1;
    EXPECT_EQ(QStringList(), m_->getMandatoryAttributesKeys(level));
}

TEST_F(medAPHPGTest, test_mandatory_attr_keys_invalid_level_failed)
{
    int level = 4;
    EXPECT_EQ(QStringList(), m_->getMandatoryAttributesKeys(level));
}

TEST_F(medAPHPGTest, test_get_minimal_entries_level_patient_failed_QtDCM_return_empty_array)
{
    QList<QMap<QString, QString>> foo;

    ON_CALL(fakeQtDCM, findPatientMinimalEntries).WillByDefault(testing::Return(foo));

    EXPECT_CALL(fakeQtDCM, findPatientMinimalEntries).Times(1);
    EXPECT_CALL(fakeQtDCM, findStudyMinimalEntries).Times(0);
    EXPECT_CALL(fakeQtDCM, findSeriesMinimalEntries).Times(0);
    EXPECT_CALL(fakeAnnotation, findAnnotationMinimalEntries).Times(0);

    QList<medAbstractSource::levelMinimalEntries> entries = m_->getMinimalEntries(0, "key");
    EXPECT_EQ(entries.size(), 0);
}

TEST_F(medAPHPGTest, test_get_minimal_entries_level_patient_success)
{
    QList<QMap<QString, QString>> foo;
    QMap<QString, QString> bar;
    bar["Name"] = "John Doe";
    bar["ID"] = "0000001";
    foo.append(bar);
    QMap<QString, QString> qix;
    qix["Name"] = "Jane Dae";
    qix["ID"] = "1000000";
    foo.append(qix);

    EXPECT_CALL(fakeQtDCM, findPatientMinimalEntries).Times(1);
    EXPECT_CALL(fakeQtDCM, findStudyMinimalEntries).Times(0);
    EXPECT_CALL(fakeQtDCM, findSeriesMinimalEntries).Times(0);
    EXPECT_CALL(fakeAnnotation, findAnnotationMinimalEntries).Times(0);

    ON_CALL(fakeQtDCM, findPatientMinimalEntries).WillByDefault(testing::Return(foo));

    QList<medAbstractSource::levelMinimalEntries> entries = m_->getMinimalEntries(0, "key");
    EXPECT_EQ(entries.size(), foo.size());

    EXPECT_EQ(entries[0].key,"0000001");
    EXPECT_EQ(entries[0].name,"John Doe");
    EXPECT_EQ(entries[0].description,"0000001");

    EXPECT_EQ(entries[1].key,"1000000");
    EXPECT_EQ(entries[1].name,"Jane Dae");
    EXPECT_EQ(entries[1].description,"1000000");

//    medAPHP *m = new medAPHP(new QtDcmAPHP(), new medAnnotation());
//    QList<medAbstractSource::levelMinimalEntries> entries = m->getMinimalEntries(0, "key");
//    for (const auto& entry : entries)
//    {
//        qDebug()<<"entry "<<entry.key<<" "<<entry.name<<" "<<entry.description;
//    }
}

TEST_F(medAPHPGTest, test_get_minimal_entries_level_study_failed_QtDCM_return_empty_arra)
{
    QList<QMap<QString, QString>> foo;
    EXPECT_CALL(fakeQtDCM, findPatientMinimalEntries).Times(0);
    EXPECT_CALL(fakeQtDCM, findStudyMinimalEntries).Times(1);
    EXPECT_CALL(fakeQtDCM, findSeriesMinimalEntries).Times(0);
    EXPECT_CALL(fakeAnnotation, findAnnotationMinimalEntries).Times(0);

    ON_CALL(fakeQtDCM, findStudyMinimalEntries).WillByDefault(testing::Return(foo));

    QList<medAbstractSource::levelMinimalEntries> entries = m_->getMinimalEntries(1, "key");

    EXPECT_EQ(entries.size(), 0);

}

TEST_F(medAPHPGTest, test_get_minimal_entries_level_study_success)
{
    QList<QMap<QString, QString>> foo;
    QMap<QString, QString> bar;
    bar["Description"] = "Study1";
    bar["StudyInstanceUID"] = "1.2.3.4.5.6666.7777777";
    foo.append(bar);
    QMap<QString, QString> qix;
    qix["Description"] = "Study2";
    qix["StudyInstanceUID"] = "1.2.3.4.5.6666.8888888";
    foo.append(qix);

    EXPECT_CALL(fakeQtDCM, findPatientMinimalEntries).Times(0);
    EXPECT_CALL(fakeQtDCM, findStudyMinimalEntries).Times(1);
    EXPECT_CALL(fakeQtDCM, findSeriesMinimalEntries).Times(0);
    EXPECT_CALL(fakeAnnotation, findAnnotationMinimalEntries).Times(0);

    ON_CALL(fakeQtDCM, findStudyMinimalEntries).WillByDefault(testing::Return(foo));
    QList<medAbstractSource::levelMinimalEntries> entries = m_->getMinimalEntries(1, "key");

    EXPECT_EQ(entries.size(), foo.size());

    EXPECT_EQ(entries[0].key,"1.2.3.4.5.6666.7777777");
    EXPECT_EQ(entries[0].name,"Study1");
    EXPECT_EQ(entries[0].description,"1.2.3.4.5.6666.7777777");

    EXPECT_EQ(entries[1].key,"1.2.3.4.5.6666.8888888");
    EXPECT_EQ(entries[1].name,"Study2");
    EXPECT_EQ(entries[1].description,"1.2.3.4.5.6666.8888888");
//    int cpt = 0;
//    for (const auto& entry : entries)
//    {
//        EXPECT_EQ(entry.key,foo[cpt]["InstanceUID"]);
//        EXPECT_EQ(entry.name,foo[cpt]["Description"]);
//        EXPECT_EQ(entry.description,foo[cpt]["InstanceUID"]);
//        cpt++;
//    }
}

TEST_F(medAPHPGTest, test_get_minimal_entries_level_series_failed_QtDCM_empty_array)
{
    QList<QMap<QString, QString>> foo;

    EXPECT_CALL(fakeQtDCM, findPatientMinimalEntries).Times(0);
    EXPECT_CALL(fakeQtDCM, findStudyMinimalEntries).Times(0);
    EXPECT_CALL(fakeQtDCM, findSeriesMinimalEntries).Times(1);
    EXPECT_CALL(fakeAnnotation, findAnnotationMinimalEntries).Times(0);

    ON_CALL(fakeQtDCM, findSeriesMinimalEntries).WillByDefault(testing::Return(foo));
//    ON_CALL(fakeAnnotation, findAnnotationMinimalEntries).WillByDefault(::testing::Return(foo));

    QList<medAbstractSource::levelMinimalEntries> entries = m_->getMinimalEntries(2, "key");

    EXPECT_EQ(entries.size(), 0);
}

TEST_F(medAPHPGTest, test_get_minimal_entries_level_series_success)
{
    QList<QMap<QString, QString>> foo;
    QMap<QString, QString> bar;
    bar["Description"] = "T2";
    bar["SeriesInstanceUID"] = "1.2.3.4.5.6666.7777777";
    foo.append(bar);
    QMap<QString, QString> qix;
    qix["Description"] = "Diffusion";
    qix["SeriesInstanceUID"] = "1.2.3.4.5.6666.8888888";
    foo.append(qix);

    EXPECT_CALL(fakeQtDCM, findPatientMinimalEntries).Times(0);
    EXPECT_CALL(fakeQtDCM, findStudyMinimalEntries).Times(0);
    EXPECT_CALL(fakeQtDCM, findSeriesMinimalEntries).Times(1);
    EXPECT_CALL(fakeAnnotation, findAnnotationMinimalEntries).Times(0);

    ON_CALL(fakeQtDCM, findSeriesMinimalEntries).WillByDefault(testing::Return(foo));

    QList<medAbstractSource::levelMinimalEntries> entries = m_->getMinimalEntries(2, "key");
    EXPECT_EQ(entries.size(), foo.size());

    EXPECT_EQ(entries[0].key,"1.2.3.4.5.6666.7777777");
    EXPECT_EQ(entries[0].name,"T2");
    EXPECT_EQ(entries[0].description,"1.2.3.4.5.6666.7777777");

    EXPECT_EQ(entries[1].key,"1.2.3.4.5.6666.8888888");
    EXPECT_EQ(entries[1].name,"Diffusion");
    EXPECT_EQ(entries[1].description,"1.2.3.4.5.6666.8888888");

}

TEST_F(medAPHPGTest, test_get_minimal_entries_level_annotation_failed_RestAPI_return_empty)
{
    QList<QMap<QString, QString>> foo;

    EXPECT_CALL(fakeQtDCM, findPatientMinimalEntries).Times(0);
    EXPECT_CALL(fakeQtDCM, findStudyMinimalEntries).Times(0);
    EXPECT_CALL(fakeQtDCM, findSeriesMinimalEntries).Times(0);
    EXPECT_CALL(fakeAnnotation, findAnnotationMinimalEntries).Times(1);

    ON_CALL(fakeAnnotation, findAnnotationMinimalEntries).WillByDefault(::testing::Return(foo));

    QList<medAbstractSource::levelMinimalEntries> entries = m_->getMinimalEntries(3, "key");

    EXPECT_EQ(entries.size(), foo.size());
}

TEST_F(medAPHPGTest, test_get_minimal_entries_level_annot_success)
{

    QList<QMap<QString, QString>> foo;
    QMap<QString, QString> bar;
    bar["Description"] = "Annot1766";
    bar["SeriesInstanceUID"] = "1.2.3.4.5.6666.7777777";
    foo.append(bar);
    QMap<QString, QString> qix;
    qix["Description"] = "Annot1900";
    qix["SeriesInstanceUID"] = "1.2.3.4.5.6666.8888888";
    foo.append(qix);

    EXPECT_CALL(fakeQtDCM, findPatientMinimalEntries).Times(0);
    EXPECT_CALL(fakeQtDCM, findStudyMinimalEntries).Times(0);
    EXPECT_CALL(fakeQtDCM, findSeriesMinimalEntries).Times(0);
    EXPECT_CALL(fakeAnnotation, findAnnotationMinimalEntries).Times(1);

    ON_CALL(fakeAnnotation, findAnnotationMinimalEntries).WillByDefault(::testing::Return(foo));

    QList<medAbstractSource::levelMinimalEntries> entries = m_->getMinimalEntries(3, "key");
    EXPECT_EQ(entries.size(), foo.size());

    EXPECT_EQ(entries[0].key,"1.2.3.4.5.6666.7777777");
    EXPECT_EQ(entries[0].name,"Annot1766");
    EXPECT_EQ(entries[0].description,"1.2.3.4.5.6666.7777777");

    EXPECT_EQ(entries[1].key,"1.2.3.4.5.6666.8888888");
    EXPECT_EQ(entries[1].name,"Annot1900");
    EXPECT_EQ(entries[1].description,"1.2.3.4.5.6666.8888888");
}