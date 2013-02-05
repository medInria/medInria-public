#!/usr/bin/perl -w
#
# Script that will move testing results from /ftp/incoming to the 
# appropriate location for dashboard summarization.  
#
# Derived from Dart.pl (the trigger CGI script normally called by the client)
#
# This needs to be called by cron before Dashboard Rollup.
# Script takes 2 parameters:
# $1: the directory where the clients drop their files (ftp-incoming)
# $2: dart's Testing/HTML/TestingResults/Sites directory
#
# not finished yet!!!

use File::Basename;
use File::Copy;
use File::Path;


$dropLocation = shift;
$destination = shift;

print "Looking for new client reports in $dropLocation\n";

opendir INDIR, $dropLocation || die ("Directory $dropLocation not found: $!\n");

while ($xmlfile = readdir (INDIR)) {

  if ($xmlfile !~ /^\./) { # if it does not start with a .
    
    $xmlfile =~ s/%(..)/sprintf("%c", hex($1))/g;   # unquote %-quoted


    $fullPathToIncomingXMLFile = $dropLocation . "/" . $xmlfile;
    if (-e $fullPathToIncomingXMLFile) {
      # file exists, so lets move it
      
      # first, translate the xml filename to a directory path
      $xmlfile =~ s|___|/|g;
      
      # for security reasons, disallow any file with ".." in the user 
      # specified path to keep people from storing files just anywhere in
      # the host directory structure
      $securityCheck = $xmlfile;
      if ( ($securityCheck =~ /\.\./) ) {
	print ("For security reasons, $xmlfile cannot be accepted.\n");
	exit;
      }
      
      # if filename ends with a date stamp (added by ftp-sop),
      # cut it off:
      $xmlname = $xmlfile;
      $xmlname =~ s/.\d\d-\d\d-\d\d\d\d-\d\d:\d\d:\d\d$//;

      # construct destination path and filename
      $fullPathToDestinationXMLFile = $destination . "/" . $xmlname;
      mkpath( dirname( $fullPathToDestinationXMLFile ) );
      
      # now copy the file to destination
      chmod(0640,$fullPathToIncomingXMLFile);
      move( $fullPathToIncomingXMLFile, $fullPathToDestinationXMLFile);
      
      print ("$xmlfile submission successful.\n");
    }
    else {
      # specified file does not exist
      print ("$xmlfile submission failed. $xmlfile is not in the dropbox.\n");
    }
  }
}
closedir INDIR;
