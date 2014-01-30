/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vtkXMLWriter.h>
#include <medVtkInriaExport.h>
#include <string>

class vtkDataManager;
class vtkMetaDataSet;
class vtkCallbackCommand;
class vtkDataManagerWriterInternals;


class MEDVTKINRIA_EXPORT vtkDataManagerWriter : public vtkXMLWriter
{
public:
  static vtkDataManagerWriter* New();
  vtkTypeRevisionMacro(vtkDataManagerWriter,vtkXMLWriter);
  void PrintSelf(ostream& os, vtkIndent indent);  
  
  // Description:
  // Get the default file extension for files written by this writer.
  virtual const char* GetDefaultFileExtension();
  
  // Description:
  // Get/Set the piece number to write.  The same piece number is used
  // for all inputs.
  vtkGetMacro(Piece, int);
  vtkSetMacro(Piece, int);
  
  // Description:
  // Get/Set the number of pieces into which the inputs are split.
  vtkGetMacro(NumberOfPieces, int);
  vtkSetMacro(NumberOfPieces, int);
  
  // Description:
  // Get/Set the number of ghost levels to be written.
  vtkGetMacro(GhostLevel, int);
  vtkSetMacro(GhostLevel, int);
  
  // Description:
  // Get/Set whether this instance will write the meta-file. 
  vtkGetMacro(WriteMetaFile, int);
  virtual void SetWriteMetaFile(int flag);


  // Description:
  // Set/Get an input of this algorithm. You should not override these
  // methods because they are not the only way to connect a pipeline
  void SetInput(vtkDataManager *);
  vtkDataManager *GetInput();


  
  // See the vtkAlgorithm for a desciption of what these do
  int ProcessRequest(vtkInformation*,
                     vtkInformationVector**,
                     vtkInformationVector*);

protected:
  vtkDataManagerWriter();
  ~vtkDataManagerWriter();
  
  // see algorithm for more info
  virtual int FillInputPortInformation(int port, vtkInformation* info);

  int RequestData(
    vtkInformation*  , vtkInformationVector** , vtkInformationVector*);
  int RequestUpdateExtent(
    vtkInformation*  , vtkInformationVector** , vtkInformationVector*);

  virtual int WriteData();
  virtual const char* GetDataSetName();

  // Create a default executive.
  virtual vtkExecutive* CreateDefaultExecutive();

  vtkInformation* InputInformation;

  vtkXMLWriter* CreateWriter(vtkMetaDataSet* metadataset);
  
  // Methods to help construct internal file names.
  void SplitFileName();
  const char* GetFilePrefix();
  const char* GetFilePath();

  // Methods to construct the list of entries for the collection file.
  void AppendEntry(const char* entry);
  void DeleteAllEntries();
  
  // Write the collection file if it is requested.
  int WriteMetaFileIfRequested();
  
  // Make a directory.
  void MakeDirectory(const char* name);
  
  // Remove a directory.
  void RemoveADirectory(const char* name);
  
  // Internal implementation details.
  vtkDataManagerWriterInternals* Internal;  
  
  // The piece number to write.
  int Piece;
  
  // The number of pieces into which the inputs are split.
  int NumberOfPieces;
  
  // The number of ghost levels to write for unstructured data.
  int GhostLevel;
  
  // Whether to write the collection file on this node.
  int WriteMetaFile;
  int WriteMetaFileInitialized;
  
  // Callback registered with the ProgressObserver.
  static void ProgressCallbackFunction(vtkObject*, unsigned long, void*,
                                       void*);
  // Progress callback from internal writer.
  virtual void ProgressCallback(vtkAlgorithm* w);
  
  // The observer to report progress from the internal writer.
  vtkCallbackCommand* ProgressObserver;  
  
  // Garbage collection support.
  virtual void ReportReferences(vtkGarbageCollector*);

//BTX  
  virtual std::string CreateMetaDataSetEntry(vtkMetaDataSet* metadataset, vtkIndent indent, unsigned int groupid, unsigned int datasetid = 0);
//ETX
  virtual int WriteMetaDataSet (vtkMetaDataSet* metadataset, unsigned int groupid, unsigned int datasetid);
  
  
private:
  vtkDataManagerWriter(const vtkDataManagerWriter&);  // Not implemented.
  void operator=(const vtkDataManagerWriter&);  // Not implemented.

  vtkDataManager* Input;
  
  
};


