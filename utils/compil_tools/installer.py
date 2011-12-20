#!/usr/bin/env python
from optparse import OptionParser
import sys,os,zipfile,ConfigParser,platform, shutil,urllib,string
from exceptions import OSError,NotImplementedError
import re,subprocess,exceptions
import types,shlex
import logging, logging.config

################################################################################
#
# Logger settings
#
################################################################################
def config_logging(log, config):
    """
    Configure the logging for the project
    """
    fileName=config.get("logging","filename")

    if (log and fileName):
        #logging.basicConfig(filename=fileName,level=logging.DEBUG)
        print "create logger from conf file:",fileName 
        logging.config.fileConfig(fileName)

        # create logger
        logger = logging.getLogger('root')
    else:
        print "create default logger" 
        # create logger
        logging.basicConfig(format='%(asctime)s - %(levelname)s - %(message)s',
                level=logging.DEBUG)
        logger = logging.getLogger('root')


################################################################################
#
# Utility functions
#
#
################################################################################

if not "check_output" in subprocess.__dict__:

    def check_output(*popenargs, **kwargs):

        if 'stdout' in kwargs:
            raise ValueError('stdout argument not allowed, it will be overridden.')
        process = subprocess.Popen(stdout=subprocess.PIPE, *popenargs, **kwargs)
        output, unused_err = process.communicate()
        retcode = process.poll()
        if retcode:
            cmd = kwargs.get("args")
            if cmd is None:
                cmd = popenargs[0]
            raise CalledProcessError(retcode, cmd, output=output)
        return output
else:
    from subprocess import check_output

def run_and_log(*popenargs, **kwargs):
    if 'stderr' in kwargs:
            raise ValueError('stderr argument not allowed, it will be overridden.')
    if 'stdout' in kwargs:
            raise ValueError('stdout argument not allowed, it will be overridden.')

    process = subprocess.Popen(stderr=subprocess.STDOUT,
                    stdout=subprocess.PIPE, *popenargs, **kwargs)
    output, unused_err = process.communicate()
    retcode = process.poll()
    if retcode:
        logging.error(output)
        return
    logging.info(output)


def _extract_all(self, destdir):
    namelist = self.namelist()
    namelist.sort()
    for name in namelist:
        if name.endswith('/'):
            logging.info(name);
            os.makedirs(os.path.join(destdir, name))
        else:
            outfile = open(os.path.join(destdir, name), 'wb')
            outfile.write(self.read(name))
            outfile.close()

def unzip(src, dest):
    """
    Not used at the moment.
    """
    zipfile.ZipFile.extract_all = _extract_all
    zipo = zipfile.ZipFile(src, 'r')
    zipo.extract_all(dest)

def reporthook(a,b,c):
    # ',' at the end of the line is important!
    print "% 3.1f%% of %d bytes\r" % (min(100, float(a * b) / c * 100), c),
    #you can also use sys.stdout.write
    #sys.stdout.write("\r% 3.1f%% of %d bytes"
    #                 % (min(100, float(a * b) / c * 100), c)
    sys.stdout.flush()

def url_get_file(url,file_path):
    """
    gets files using a url.
    @param url: address where to fetch the file.
    @param file: destination file.
    """
    #i = url.rfind('/')
    #file = url[i+1:]
    logging.info( "%s -> %s",url, file_path)
    urllib.urlretrieve(url, file_path, reporthook)
    sys.stdout.flush()
    return

def find_architecture():
    """
    Gets the architecture the script is running on.
    @warning: Under windows on Pipol machines the architecture returned is
    CYGWIN.
    @TODO: deal with cygwin strings
    @returns: the architecture we are compiling on.
    """
    ARCHITECTURE = ''

    if sys.platform.find('darwin') != -1:
        ARCHITECTURE='mac'
    elif sys.platform.find('win') != -1:
        ARCHITECTURE='win'
    else:
        ARCHITECTURE='linux'
        if platform.architecture()[0].find('64') != -1:
            ARCHITECTURE=ARCHITECTURE+'64'
        else:
            ARCHITECTURE=ARCHITECTURE+'32'
    logger = logging.getLogger('root')
    logger.info( 'found the architecture : %s',ARCHITECTURE)
    logger.info("test")
    return ARCHITECTURE

def install_package_deps(config):
    """
    Installs packages we depends on using a package management system.
    The packge management depends on the config file's specifications.
    """
    manager = config.get("package_deps","package_manager")
    if manager == "yum":
        cmd= "yum install -y "+ config.get("package_deps","deps")
    elif manager == "apt":
        cmd = "apt-get install -y " + config.get("package_deps","deps")
    elif manager == '':
        cmd=""
    else:
        raise "unknown package manager"
    if not cmd: return
    logging.info( cmd)
    run_and_log(cmd.split())

def load_config_files(filenames=[]):
    """
    Loads config files.
    Loads the file default_param.cfg first. It has to be in the directory
    from which the script is run.
    Then loads a list of paths. the tail of the list always overrides the head.
    @returns:a L{ConfigParser} object.
    """
    config=ConfigParser.ConfigParser()
    config.optionxform = str
    config.readfp(open("default_param.cfg"))
    print(config.read(filenames))
    return config

def configure_project(project,config,architecture='linux'):
    """
    Run the configure command on the project.
    The command may be cmake or ./configure, depending on the config.
    Let's assume we are in the project's directory.
    """
    command=""
    if config.get(project,"configure_type")== "cmake":
        cmake_command=config.get("commands","cmake")

        #if not os.path.exists(os.path.abspath('build')):

        src_dir = os.path.join(config.get("DEFAULT","projects_dir"),
                               config.get(project,"destination_dir"))
        build_dir = config.get(project,"build_dir")
        if (architecture == "win"):
            src_dir=config.get(project,"cyg_drive")+src_dir
        shutil.rmtree(build_dir,True)
        print("making dir %s",build_dir )
        os.makedirs(build_dir)
        os.chdir(build_dir)

        command= shlex.split(cmake_command)
        for k,v in config.items("cmake-common"):
            #prevent defaults to pollute the command line
            if not config.has_option("DEFAULT",k):
                command.append('-D'+k+':'+v)
        for k,v in config.items(project+"-cmake"):
            if not config.has_option("DEFAULT",k):
                command.append('-D'+k+':'+v)
        command.append(src_dir)
    elif config.get(project,"configure_type")== "autoconf":
        command = [os.path.abspath("./configure")]
        for k,v in config.items(project+"-configure"):
            #prevent defaults to pollute the command line
            if not config.has_option("DEFAULT",k):
                command.append(k+"="+v)

    logging.info( command)
    run_and_log(command)

def build(project,config,architecture="linux"):
    """
    Run the build command on the project.
    Let's assume we are in the project's directory.
    """
    #build_dir = os.path.join(config.get("DEFAULT","projects_dir"),project,"build")
    project_name=config.get(project,"project_name")
    os.chdir(config.get(project,"build_dir"))
    make_command = config.get("commands","make")
    if architecture == 'win' or architecture == 'win32' or architecture == 'win64':
        win32_config= config.get(project,"win32_configuration")

        if not win32_config and config.get(project,"build_type")=="Release":
            win32_config = "Release\|Win32"

        generator_type = config.get(project,"generator_type")
        if generator_type == 'NMake Makefiles':
                cmd = make_command + ' -f Makefile'
        else:
            cmd =  config.get("commands",
                    "visual_build") +' ' + project_name + '.sln ' + win32_config
    else:
         cmd = make_command+' -j'+config.get(project,"cpus") + ' -k'
    logging.info(cmd)
    run_and_log(cmd.split())

    extra_build_cmd=config.get(project,"extra_build_cmd")
    if len(extra_build_cmd):
        logging.info(extra_build_cmd)
        run_and_log (extra_build_cmd.split())

    os.chdir('..')

    return

def test(project,config,architecture="linux"):
    """
    Run the build command on the project.
    Let's assume we are in the project's directory.
    """
    #build_dir = os.path.join(config.get("DEFAULT","projects_dir"),project,"build")
    project_name=config.get(project,"project_name")
    os.chdir(config.get(project,"build_dir"))
    # make_command = config.get("commands","make")
    ctest_build_type=config.get("DEFAULT","ctest_build_type")
    ctest_command = config.get("commands","ctest")
    cmd = ctest_command + " -D " + ctest_build_type
    logging.info(cmd)
    run_and_log(cmd.split())

    extra_build_cmd=config.get(project,"extra_build_cmd")
    if len(extra_build_cmd):
        logging.info(extra_build_cmd)
        run_and_log (extra_build_cmd.split())

    os.chdir('..')

    return
    
    
def uncomp_tar(project,config,file):
    """
    Uncompresses a file to the corresponding project's destination_dir
    """
    import tarfile
    tar = tarfile.open(file)
    dest_dir = config.get(project,"destination_dir")
    tar.extractall(dest_dir)
    tar.close()
    return



def wget(project,config):
    """
    Gets a source package from the web and Uncompress it if needed.
    Uses the "wget" and the uncompress_command specified in
    the config file under the project's section.

    If the wget command is "python",
    then a python function is called to download the file,
    Only if it doesn't exist yet.
    """
    wget_c = config.get("commands","wget")

    source_file = config.get(project,"source_file")
    source = config.get(project,"source_host")+"/"+source_file
    archive_dir = config.get(project,"archive_dir")
    target_file = os.path.join(archive_dir,source_file)
    logging.info("arc_dir: %s, file: %s",archive_dir,target_file)

    if wget_c == "python":
        if not os.path.isfile(target_file):
            url_get_file(source,target_file)
        else:
            logging.warn("file %s already exists, skipping download",target_file)
    else:
        wget_c = wget_c + " " + source
        run_and_log(wget_c.split())
    #uncompress in the right directory...
    uncomp_cmd = config.get(project,"uncompress_command")
    logging.info(uncomp_cmd)
    #len(python_)=7
    if len(uncomp_cmd)>7 and uncomp_cmd[0:7]== "python_" :
        option = uncomp_cmd[7:]
        logging.info( "uncompress file type:%s",option)
        if option == "tar":
            uncomp_tar(project,config,target_file)
    else:
        run_and_log(uncomp_cmd.split())
    return

def _git_path(project,config):
    """
    Correctly setup the path according to the git host and username.
    Handles the default protocol (ssh)
    """
    scm_protocol =  config.get(project,"scm_protocol")
    source_host = config.get(project,"source_host")
    source_file = config.get(project,"source_file")
    user = config.get(project,"username")
    user_item=""
    if len(user) > 0  :
        user_item=user+"@"
    if scm_protocol == "git":
        path = "git://"+source_host+'/'+ source_file
    elif scm_protocol == "ssh":
        path = user_item + source_host +':'+ source_file
    else:
        path = scm_protocol+"://"+source_host +'/'+ source_file
    return path

def _svn_path(project,config):
    """
    Correctly setup the path according to the git host and username.
    Handles the default protocol (ssh)
    """
    scm_protocol =  config.get(project,"scm_protocol")
    source_host = config.get(project,"source_host")
    source_file = config.get(project,"source_file")
    user = config.get(project,"username")
    user_item=""
    if len(user) > 0  :
        user_item=user+"@"
    if scm_protocol == "ssh":
        path = "svn+ssh://"+user_item + source_host +'/'+ source_file
    else:
        path = source_host +'/'+ source_file
    return path


def git_clone(project,config):
    """
    Gets the project's source code using git and checks out a specific branch.
    """
    git_command = config.get("commands","git")
    dest_dir = config.get(project,"destination_dir")
    url = _git_path(project,config)
    logging.debug(url)
    cmd = git_command + " clone "+ url + " " + dest_dir
    logging.debug(cmd)
    run_and_log(cmd.split())
    os.chdir(dest_dir)
    _git_checkout(project, config)
    os.chdir("..")
    return

def git_pull(project,config):
    """
    Gets the project's source code using git and checks out a specific branch.
    """
    git_command = config.get("commands","git")
    dest_dir = config.get(project,"destination_dir")
    os.chdir(dest_dir)
    cmd = git_command.split()
    cmd.extend(["checkout","master"])
    run_and_log(cmd)
    cmd = git_command.split()
    cmd.append("pull")
    logging.debug(cmd)
    run_and_log(cmd)
    _git_checkout(project, config)
    os.chdir("..")
    return

def _git_parse_branches(git_output):
    """
    Parses the output of 'git branch' for the active branch.  The
    output is a list of pairs of the form (active:bool, name:string).
    The active flag is the branch marked with the star in the git
    output.  The name may contain several shlashes,
    e.g. 'origin/master'.
    """

    def _parse_branch_line(line):
        p = re.compile(r"(\*?)\s+((?:.+/)?([^/]+))")
        active, full, name = p.match(line).groups()
        return (active == '*', full, name)

    return [_parse_branch_line(l) for l in git_output.splitlines()]

def _git_checkout(project, config):
    """
    Checks out a git branch (local or remote).  If the target doesn't
    exist yet, a new branch is created.  The new branch tracks the
    remote branch of the same name if existing, otherwise the active
    local branch.
    """
    git_command = config.get("commands", "git")

    git_output      = check_output([git_command,"branch"])
    local_branches  = _git_parse_branches(git_output)
    git_output      = check_output([git_command,"branch","-r"])
    remote_branches = _git_parse_branches(git_output)

    target          = config.get(project,"branch")
    active          = [full for act, full, name in local_branches if act]
    local_target    = [full for act, full, name in local_branches  \
                           if full == target]
    remote_target   = [full for act, full, name in remote_branches \
                           if full == target]
    remote_tracking = [full for act, full, name in remote_branches \
                           if name == target]

    git_co_com = git_command + " checkout "
    if local_target or remote_target:
        # target is found (local or remote)
        git_co_com += target
    else:
        # target is not found, create new branch
        git_co_com += "-b " + target
        if remote_tracking:
            # new branch tracks remote branch of same name
            git_co_com += " " + remote_tracking[0]
        else:
            # new branch tracks active local branch
            git_co_com += " " + active[0]

    # print git_command
    run_and_log(git_co_com.split())

    return

def svn_update(project,config):
    """
    Updates the project's source code using svn.
    """
    svn_command = config.get("commands","svn")
    dest_dir = config.get(project,"destination_dir")
    os.chdir(dest_dir)
    cmd = svn_command.split()
    cmd.append("update")
    run_and_log(cmd)
    os.chdir('..')
    return

def svn_checkout(project,config):
    """
    Gets the project's source code using svn.
    @warning: always downloads the whole directory, does not perform updates.
    """

    #os.makedirs(config.get(project,"destination_dir"))
    #os.chdir()
    svn_command = config.get("commands","svn")
    path = _svn_path(project,config)
    dest_dir =  config.get(project,"destination_dir")

    svn_cmd=[svn_command,'checkout', path, dest_dir]
    logging.info( svn_cmd)
    run_and_log(svn_cmd)
    return

def create_dirs(project,config):
    """
    Creates and populates source directories. SVN, wget or git.
    """

    #TODO add option to not re-clone the directory if wanted
    #if not os.path.exists(os.path.abspath(project)):
    shutil.rmtree(config.get(project,"destination_dir"),True)

    scm = config.get(project,"scm")
    if scm == "svn":
        svn_checkout(project,config)
    elif scm == "git":
        git_clone(project,config)
    elif scm == "wget":
        wget(project,config)
    else:
        raise "don't know how to get the source code for : " + project
    extra_create_dirs_cmd=config.get(project,"extra_create_dirs_cmd")
    if len(extra_create_dirs_cmd):
        logging.info( extra_create_dirs_cmd)
        run_and_log(extra_create_dirs_cmd,shell=True)

    return

def update_dirs(project,config):
    """
    Updates the directories according to the scm  (git pull or svn checkout)
    We will assume we are in the projects' directory.
    """
    scm = config.get(project,"scm")

    if os.path.exists(config.get(project,"destination_dir")):
        if scm == "svn":
            svn_update(project,config)
        elif scm == "git":
            git_pull(project,config)
        else :
            return
    else : create_dirs(project,config)

    return

def install(project,config,architecture):
    """
    Runs the install command on the project. make, nmake or visual studio
    based.
    """
    os.chdir(config.get(project,"build_dir"))
    make_command = [config.get("commands","make")]
    cmd = list()
    if architecture == 'win' or architecture == 'win32' \
            or architecture == 'win64':

        win32_config= config.get(project,"win32_configuration").split()

        if not win32_config and config.get(project,"build_type")=="Release":
            win32_config = ["Release\|Win32"]

        generator_type = config.get(project,"generator_type")
        if generator_type == 'NMake Makefiles':
            cmd.extend(make_command)
            cmd.extend(['-f', 'Makefile', 'install'])
        elif not (-1 == string.find(generator_type,"Visual Studio 10")):
            cmd.extend  ([config.get("commands",
                    "visual_build"), 'install.vcxproj'])
            cmd.extend(win32_config)
        else :
            cmd.extend ([config.get("commands",
                    "visual_build"),'install.vcproj'])
            cmd.extend(win32_config)

    else:
        cmd.extend(make_command)
        cmd.append('install')

    logging.info( cmd)
    run_and_log(cmd)

    """extra_build_cmd=config.get(project,"extra_build_cmd")
    if len(extra_build_cmd):
        print "#####\n#######\n######\n######\n######\n######\n######\n####\n"
        print extra_build_cmd
        os.system(extra_build_cmd)
    """
    os.chdir('..')

def build_package(project,config,architecture):
    """
    Runs the package creation command on the project. Only cpack for now.
    """

    logging.info("build package: " + project)
    os.chdir(config.get(project,"build_dir"))
    logging.debug(architecture)
    if architecture =='win' or architecture == 'win32' \
            or architecture == 'win64':

        cmd = [config.get("commands","make"),"package"]
        logging.info(cmd)
        # do not run the make package on this platform, only the extra_package_command
        #run_and_log(cmd)
    else:
        cpack_cmd = config.get("commands","cpack")
        pkg_mngr = config.get("package_deps","package_manager")
        logging.info( pkg_mngr)
        gen = "RPM"
        if pkg_mngr == "apt":
            gen = "DEB"
        cmd = [cpack_cmd, "-G ", gen]
        run_and_log(cmd)

    extra_package_cmd=config.get(project,"extra_package_cmd")
    if len(extra_package_cmd):
        logging.info( "Execute extra package command:"+extra_package_cmd)
        run_and_log(extra_package_cmd,shell=True)
    return

def doc(project,config,architecture):
    """
    Runs the documentation command.
    @warnign: Only linux for now.
    """

    print "doc package: " + project
    os.chdir(config.get(project,"build_dir"))

    make = config.get("commands","make")
    doc = config.get(project,"doc_command")
    doc_output = run_and_log([make,doc])
    #print doc_output

    extra_doc_cmd=config.get(project,"extra_doc_cmd")
    if len(extra_doc_cmd):
        logging.info( "Execute extra doc command: "+ extra_doc_cmd)
        run_and_log(extra_doc_cmd,shell=True)
    return


def confirm_action( prompt = None ):
    """prompts for response from the user. Returns True for continue and
    False for skip, exits for abort.
    """

    if prompt is None:
        prompt = 'Continue, Skip or Abort?'

    prompt = '%s ([c],s,a): ' % prompt

    while True:
        ans = raw_input(prompt)
        if not ans:
            ans = 'c'

        if ans not in ['c', 'C', 's', 'S', 'a', 'A']:
            print 'please enter c, s, or a (just hit enter for c).'
            continue

        if ans == 'c' or ans == 'C':
            return True
        if ans == 's' or ans == 'S':
            return False
        if ans == 'a' or ans == 'A':
            logging.shutdown()
            sys.exit()


################################################################################
#
#
#  Command line argument handling + logic
#
#
################################################################################
def main(argv):
    #generate everything in the config file.
    version = "%prog 2.1"
    usage = "usage %prog [options]"
    parser = OptionParser(usage=usage, version = version)

    parser.add_option("-f","--config-files", dest="filenames",action="append",
            #default="",type="string",
            help="configuration filename. You may use -f as many times as you \
                    like. Example: '-ffilename1 -ffilename2'\n\
                    The config files will be loaded in the order the user \
                    gave, and will override each other.")

    # package dependencies
    parser.add_option("-d","--package-deps", dest="pkg_dep",action="store_true",
            default=True,
            help="Get required distribution packages first (enabled by default)")
    parser.add_option("--no-package-deps", dest="pkg_dep",action="store_false",
            default=True,
            help="Do not Get required distribution packages first")

    # create directories
    parser.add_option("--create-dirs", dest="create_dirs",action="store_true",
            default=False,help="Create projects' directories and download \
            sources (disabled by default)")
    parser.add_option("--no-create-dirs", dest="create_dirs",
            action="store_false",
            default=False,
            help="Do not Create projects' directories and \
            do not download sources")

    # update directories
    parser.add_option("--update-dirs", dest="update_dirs",action="store_true",
            default=True,help="Update projects' directories and download \
            sources if non existing (enabled by default)")
    parser.add_option("--no-update-dirs", dest="update_dirs",
            action="store_false",
            default=True,
            help="Do not update projects' directories")

    # configure projects
    parser.add_option("--configure", dest="configure",action="store_true",
            default=True,help="configure projects (run cmake) (enabled \
                    by default)")
    parser.add_option("--no-configure", dest="configure",action="store_false",
            default=True,
            help="Do not configure projects (do not run cmake)")

    # update projects
    parser.add_option("-b","--build", dest="build",action="store_true",
            default=True,
            help="Build the projects (enabled by default)")
    parser.add_option("--no-build", dest="build",action="store_false",
            default=True,
            help="Do not build the projects")

    parser.add_option("-t","--test", dest="test",action="store_true",
            default=False,
            help="Run CTest on the projects (enabled by default)")
    parser.add_option("--no-test", dest="test",action="store_false",
            default=True,
            help="Do not run ctest on the projects")
            
    # install projects
    parser.add_option("-i","--install", dest="install",action="store_true",
            default=True,
            help="Install the projects (enabled by default)")
    parser.add_option("--no-install", dest="install",action="store_false",
            default=True,
            help="Do not install the projects")

    # package projects
    parser.add_option("-p","--package", dest="package",
            action="store_true",
            default=True,
            help="create the packages for the projects (enabled by default)")
    parser.add_option("--no-package", dest="package",
            action="store_false",
            default=True,
            help="Do not create packages for the projects")
    # build doc
    parser.add_option("-o","--doc", dest="doc",
            action="store_true",
            default=False,
            help="build the docs for the projects (disabled by default)")


    # interactive mode
    parser.add_option("--interactive",
                      dest = "interactive",
                      action = "store_true",
                      default = False,
                      help = "Prompt the user for confirmation after each task \
                              before proceeding (disabled by default)"
                      )
    parser.add_option("--no-interactive",
                      dest = "interactive",
                      action = "store_false",
                      default = False,
                      help = "Continue with next task immediately after \
                              completion of previous task."
                      )
    parser.add_option("-s","--show-conf",
                      dest = "show_conf",
                      action = "store_true",
                      default = False,
                      help = "Shows the concatenation of all config files. \
                              Debugging tool. Returns right after \
                              displaying the configuration."
                      )
    parser.add_option("-l","--log",
                      dest = "log",
                      action = "store_true",
                      default = False,
                      help = "Log all output in a file instead of the console"
                      )


    (options, args) = parser.parse_args()
    if None == options.filenames:
        filenames = list()
    else: filenames = options.filenames


    #    filenames = options.filenames.split(":")
    config = load_config_files(filenames)
    # configure logging
    config_logging(options.log,config)

    architecture = find_architecture()


    # debug function: shows config values:
    if options.show_conf:
        for section in config.sections():
            print "["+section+"]"
            for k,v in config.items(section):
                print k+"="+str(v)

    # install dependencies through the package management system.
    if options.pkg_dep and config.getint('package_deps', 'install') == 1:
        logging.info( "installing package dependencies...")
        install_package_deps(config)
    else:
        logging.info("skip installation of package dependencies")


    # create main directory
    projects_dir = os.path.abspath(config.get("DEFAULT","projects_dir"))
    archive_dir = os.path.abspath(config.get("DEFAULT","archive_dir"))
    try:
        os.makedirs(projects_dir)
    except OSError,e:
        logging.debug( e)
        logging.debug( '...no problem, continuing')
    try:
        os.makedirs(archive_dir)
    except OSError,e:
        logging.debug( e)
        logging.debug( '...no problem, continuing')

    # run the requested actions for each project
    projects_str = config.get("projects","projects")
    projects = [p.strip() for p in projects_str.split(",")]
    for project in projects :
        interactive = options.interactive or \
            config.getint( project, "interactive" ) == 1
        confirm_fun = interactive \
            and (lambda: confirm_action()) \
            or (lambda: True)
        choose_fun = lambda name: \
            getattr(options, name) and config.getint(project, name) == 1

        os.chdir(projects_dir)

        if choose_fun('create_dirs'):
            logging.info( "Creating directory for " + project + "...")
            if confirm_fun():
                create_dirs(project,config)
        elif choose_fun('update_dirs'):
            logging.info("Updating directory for " + project + "...")
            if confirm_fun():
                update_dirs(project,config)

        extra_update_cmd=config.get(project,"extra_update_cmd")
        if len(extra_update_cmd):
            logging.info( extra_update_cmd)
            run_and_log(extra_update_cmd,shell=True)
                
        cwd = os.path.join(projects_dir, config.get(project,"destination_dir"))
        os.chdir(cwd)
        if choose_fun('configure'):
            logging.info( "configuring " + project + "...")
            if confirm_fun():
                configure_project(project,config,architecture)

        os.chdir(cwd)
        if choose_fun('build') and not choose_fun('test'):
            logging.info( "building " + project + "...")
            if confirm_fun():
                build(project,config,architecture)
        elif not choose_fun('build') and choose_fun('test'):
            logging.info( "ctest " + project + "...")
            if confirm_fun():
                test(project,config,architecture)
        elif choose_fun('build') and choose_fun('test'):
            logging.error("Can't build and run CTest at the same time")
            
        os.chdir(cwd)
        if choose_fun('doc'):
            logging.info( "doc " + project + "...")
            if confirm_fun():
                doc(project,config,architecture)


        os.chdir(cwd)
        if choose_fun('install'):
            logging.info( "installing " + project + "...")
            if confirm_fun():
                install(project,config,architecture)

        os.chdir(cwd)
        if choose_fun('package'):
            logging.info( "packaging " + project + "...")
            if confirm_fun():
                build_package(project,config,architecture)

        os.chdir(cwd)

        logging.info(project)
    return


if __name__ == "__main__":
    try:
        main(sys.argv)
    except exceptions.Exception, e:
        logging.error(e)
    logging.shutdown()
