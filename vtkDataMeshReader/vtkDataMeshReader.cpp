#include <vtkDataMeshReader.h>

#include <vtkDataSetReader.h>
#include <vtkSmartPointer.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

const char vtkDataMeshReader::ID[] = "vtkDataMeshReader";

vtkDataMeshReader::vtkDataMeshReader(): dtkAbstractDataReader() {
    reader = vtkDataSetReader::New();
}

vtkDataMeshReader::~vtkDataMeshReader() {
    reader->Delete();
}

QStringList vtkDataMeshReader::handled() const {
    return QStringList() << "vtkDataMesh";
}

QStringList vtkDataMeshReader::s_handled() {
    return QStringList() << "vtkDataMesh";
}

bool vtkDataMeshReader::canRead(const QString& path) {
    reader->SetFileName(path.toAscii().constData());

    return reader->IsFilePolyData()         ||
           reader->IsFileUnstructuredGrid() ||
           reader->IsFileStructuredGrid()   ||
           reader->IsFileRectilinearGrid();
}

bool vtkDataMeshReader::canRead(const QStringList& paths) {
    if (!paths.count())
        return false;
    return canRead(paths[0].toAscii().constData());
}

void vtkDataMeshReader::readInformation(const QString& path) {

    dtkSmartPointer<dtkAbstractData> dtkdata = data();
    reader->SetFileName(path.toAscii().constData());

    if (!dtkdata) {
        dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer("vtkDataMesh");
        if (dtkdata)
            setData(dtkdata);
    }


    dtkdata->addMetaData("FilePath", QStringList() << path);
    dtkdata->identifier() = "vtkDataMesh";  //  Strange !!
}

void vtkDataMeshReader::readInformation(const QStringList& paths) {
    if (!paths.count())
        return;
    readInformation(paths[0].toAscii().constData());
}

bool vtkDataMeshReader::read(const QString& path) {
    setProgress(0);
    readInformation(path);
    setProgress(50);

    qDebug() << "Can read with: " << identifier();

    if (dtkAbstractData *dtkdata = data()) {

        if (!(dtkdata->identifier()=="vtkDataMesh"))
            return false;

        reader->SetFileName(path.toAscii().constData());
        reader->Update();
        dtkdata->setData(reader->GetOutput());
    }

    setProgress(100);
    return true;
}

bool vtkDataMeshReader::read(const QStringList& paths) {
    if (!paths.count())
        return false;
    return read(paths[0].toAscii().constData());
}

void vtkDataMeshReader::setProgress(int value) {
    emit progressed(value);
}

bool vtkDataMeshReader::registered() {
    return dtkAbstractDataFactory::instance()->registerDataReaderType(ID,
                                    vtkDataMeshReader::s_handled(),
                                    createVtkDataMeshReader);
}

QString vtkDataMeshReader::identifier() const {
    return ID;
}

QString vtkDataMeshReader::description() const {
    return "Reader for vtk meshes";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createVtkDataMeshReader() {
    return new vtkDataMeshReader;
}

