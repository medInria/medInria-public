#! /bin/sh
#
# script for MIPS dashboard
# 
# this script rolls up the dashboard, that is,
# it recreates the dashboards HTML files from the
# build result submissions that have arrived since
# the opening of the dashboard.
#
# this should be run by CRON on linux hourly after the 
# dashboard has been opened.
#
SCRIPTDIR=/u/epione/home/dashboardBuilds/vtkINRIA3D/src/Scripts
XMLLOCATION=/net/servers/ftp-sop/asclepios/Incoming/dashboard/vtkINRIA3D
XMLDESTINATION=/net/servers/www-sop/asclepios/software/vtkINRIA3D/dashboard
{
    echo "+++++++++++++++++++ ENVIRONNEMENT +++++++++++++++++++++++++++++++"
    env
    echo "------------------- ENVIRONNEMENT -------------------------------"
    # Print current date to logfile
    echo -n "Dashboard rollup started: "
    date

    # collect client reports with perl script:
    $SCRIPTDIR/dashboard_collectresults.pl $XMLLOCATION $XMLDESTINATION/Testing/HTML/TestingResults/Sites

    cd $XMLDESTINATION

    # (re)create Dashboard
    make NightlyDashboardEnd

} 2>>$XMLDESTINATION/dashboard_error.log >>$XMLDESTINATION/dashboard.log
# syncproj the dashboard only
#sudo syncproj $MIPS_DASHBOARD_DIR >/dev/null  2>&1