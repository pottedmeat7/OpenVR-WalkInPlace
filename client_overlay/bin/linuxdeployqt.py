#!/usr/bin/env python

'''
The MIT License (MIT)
Copyright (c) 2016-2018 Lars Pontoppidan

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

'''

'''


import os
import sys
import json
import subprocess
import argparse
import tempfile

import shutil, errno

script_path = os.path.dirname(os.path.realpath(__file__)).rstrip(os.sep)

parser = argparse.ArgumentParser(description='Deploy a Qt application on linux.')

parser.add_argument('executable', help='Input executable')
#parser.add_argument('-i','--input', help='Input executable', required=False)
parser.add_argument('-qt','--qt-base-dir', help="Qt's base install path.", required=True)
parser.add_argument('-qtqml','--qt-qml-dir', help="Path to Qt's QML imports directory. If not specified 'QT_BASE_DIR/qml' is used.", required=False, default="")
parser.add_argument('-qtbin','--qt-bin-dir', help="Path to Qt's 'bin' directory. If not specified 'QT_BASE_DIR/bin' is used.", required=False, default="")
parser.add_argument('-qtplugins','--qt-plugin-dir', help="Path to Qt's 'plugins' directory. If not specified 'QT_BASE_DIR/plugins' is used.", required=False, default="")
parser.add_argument('--qml-import', help="Additional QML imports. Specify as '\"<module> <version>\"' e.g --qml-import \"QtMultimedia 5.0\". This is especially useful if you include QML code dynamically with e.g. Qt.createQmlObject(\"import QtMultimedia 5.0; SoundEffect{ ... }\")", action='append', required=False)
parser.add_argument('-s','--qml-scan-dir', help='Directory to be recursively scanned for QML file imports. This argument can be specified multiple times to include more directories.', action='append', required=True)
parser.add_argument('-o','--output', help="Output dependencies to JSON file. Use '-' for stdout", required=False)
parser.add_argument('-v','--verbose', help='Verbose level <0-3> where 0 = No output. 1 = Info. 2 = Info+Warnings. 3 = Info+Warnings+Debug', type=int, default=0)

parser.add_argument('-ad','--appdir', help="Deploy to an AppImageKit 'AppDir'. If no argument is given a default location in /tmp will be used.", nargs='?', default='', required=False)
parser.add_argument('-ai','--appimage', help="Deploy to an AppImageKit 'AppImage'. If no argument is given a default location in /tmp will be used. This also automatically sets '--appdir' to a temporary location if not specified.", nargs='?', default='', required=False)
parser.add_argument('-ib','--ignore-blacklist', help="Turn off the blacklist. No libraries will be filtered.", required=False, action='store_true')

parsed_args = vars(parser.parse_args())

# Arguments minus script name
args = sys.argv[1:]

# Try to see if first argument is an executable file
#exe = os.path.abspath(args[0])
#if which(exe) is None:
exe = os.path.realpath(parsed_args['executable']) #os.path.abspath(args[0])

qt_install_dir = os.path.realpath(parsed_args['qt_base_dir']) #os.path.abspath(args[1])

qml_scan_dirs = parsed_args['qml_scan_dir'] #os.path.abspath(args[2])
for index, elem in enumerate(qml_scan_dirs):
    qml_scan_dirs[index] = os.path.abspath(elem)

qt_qml_dir = qt_install_dir+os.sep+'qml'
qt_bin_dir = qt_install_dir+os.sep+'bin'
qt_plugin_dir = qt_install_dir+os.sep+'plugins'

qml_manual_imports = parsed_args['qml_import']

app_dir = os.path.join(tempfile.gettempdir(),os.path.basename(exe)+".AppDir")
do_build_app_dir = False
app_image = os.path.join(tempfile.gettempdir(),os.path.basename(exe)+".AppImage")

do_build_app_image = False

if parsed_args['appdir'] != '':
    do_build_app_dir = True
    if parsed_args['appdir'] is not None:
        app_dir = os.path.abspath(parsed_args['appdir'])

if parsed_args['appimage'] != '':
    do_build_app_dir = True
    do_build_app_image = True
    if parsed_args['appimage'] is not None:
        app_image = os.path.abspath(parsed_args['appimage'])

#output_file = './'+os.path.basename(exe)+'-dependencies.json'
output_file = ''
if parsed_args['output'] is not None:
    output_file = parsed_args['output']
    output_file = output_file.strip(' \t\n\r')
    if output_file != '-':
        output_file = os.path.abspath(output_file)

# temporary directory to work in
tmp_dir = os.path.join(tempfile.gettempdir(),"linuxdeployqt.py.tmp")
if os.path.exists(tmp_dir):
    shutil.rmtree(tmp_dir)
os.makedirs(tmp_dir)

# Helper functions
def error(s):
    print("ERROR: "+s)
    exit(1)

def warn(s):
    if parsed_args['verbose'] > 1:
        print("WARNING: "+s)

def info(s):
    if parsed_args['verbose'] > 0:
        print("INFO: "+s)

def debug(s):
    if parsed_args['verbose'] > 2:
        print("DEBUG: "+s)


blacklist = [
    'linux-vdso.so.1',
    'ld-linux-x86-64.so.2'
]

if not parsed_args["ignore_blacklist"]:
    blacklist_url = 'https://raw.githubusercontent.com/probonopd/AppImages/master/excludelist'
    info('Updating blacklist from '+blacklist_url)
    update_blacklist_cmd = 'wget --quiet '+blacklist_url+' -O - | sort | uniq | grep -v "^#.*" | grep "[^-\s]"'
    blacklist += os.popen(update_blacklist_cmd).read().split('\n')
else:
    blacklist = []
#print(blacklist)
#exit(0)


def merge_dicts(x, y):
    '''Given two dicts, merge them into a new dict as a shallow copy.'''
    z = x.copy()
    z.update(y)
    return z

def which(program):
    '''Determine if argument is an executable and return the full path. Return None if none of either.'''
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None

def resolve_dependencies(executable):
    # NOTE Use 'ldd' method for now.
    # TODO Use non-ldd method (objdump) for cross-compiled apps
    return ldd(executable)
    #objdump(executable)
    return {}

def objdump(executable):
    '''Get all library dependencies (recursive) of 'executable' using objdump'''
    libs = {}
    return objdumpr(executable,libs)

def objdumpr(executable,libs):
    '''Get all library dependencies (recursive) of 'executable' using objdump'''
    try:
        output = subprocess.check_output(["objdump", "-x", executable])
    except subprocess.CalledProcessError as e:
        warn("CalledProcessError while running %s. Return code %s - output: %s" % (e.cmd,e.returncode,e.output))
        output = e.output

    output = output.split('\n')

    accepted_columns = [ 'NEEDED','RPATH','RUNPATH' ]

    for line in output:
        split = line.split()
        if len(split) == 0:
            continue

        if split[1] not in accepted_columns:
            continue

        if split[1] in blacklist or os.path.basename(split[0]) in blacklist:
            debug("'%s' is blacklisted. Skipping..." % (split[0]))
            continue

        so = split[1]
        path = split[2]
        realpath = os.path.realpath(path)

        if not os.path.exists(path):
            debug("Can't find path for %s (resolved to %s). Skipping..." % (so,path))
            continue

        if so not in libs:
            details = { 'so':so, 'path':path, 'realpath':realpath, 'dependants':set([executable]), 'type':'lib' }
            libs[so] = details

            debug("Resolved %s to %s" % (so, realpath))

            libs = merge_dicts(libs, lddr(realpath,libs))
        else:
            libs[so]['dependants'].add(executable)

    return libs


def ldd(executable):
    '''Get all library dependencies (recursive) of 'executable' '''
    libs = {}
    return lddr(executable,libs)

def lddr(executable,libs):
    '''Get all library dependencies (recursive) of 'executable' '''
    try:
        output = subprocess.check_output(["ldd", "-r", executable])
    except subprocess.CalledProcessError as e:
        warn("CalledProcessError while running %s. Return code %s - output: %s" % (e.cmd,e.returncode,e.output))
        output = e.output

    output = output.split(b'\n')

    for line in output:
        split = line.split()
        if len(split) == 0:
            continue

        if split[0] in blacklist or os.path.basename(split[0]) in blacklist:
            debug("'%s' is blacklisted. Skipping..." % (split[0]))
            continue

        if split[0] == 'statically' and split[1] == 'linked':
            debug("'%s' is statically linked. Skipping..." % (os.path.basename(executable)))
            continue

        if len(split) < 3:
            warn("Could not determine path of %s %s for ldd output line '%s'. Skipping..." % (os.path.basename(executable),split,line))
            continue

        so = split[0]
        path = split[2]
        realpath = os.path.realpath(path)

        if not os.path.exists(path):
            debug("Can't find path for %s (resolved to %s). (%s => %s) Skipping..." % (so,path,executable,line))
            continue

        if so not in libs:
            details = { 'so':so, 'path':path, 'realpath':realpath, 'dependants':set([executable]), 'type':'lib' }
            libs[so] = details

            debug("Resolved %s to %s" % (so, realpath))

            libs = merge_dicts(libs, lddr(realpath,libs))
        else:
            libs[so]['dependants'].add(executable)

    return libs

def qml_imports(path, lib_path):
    '''Find QML dependencies from QML files in 'path' and pair them with libraries in 'lib_path' '''

    # So ... qmlimportscanner has - or has had - a bad reputation.
    # It's recursive auto discovery from -rootPath has given wrong results in various Qt versions
    # If you trust the output of qmlimportscanner use this instead of the code below:
    #qmlscanner_args = [qt_bin_dir+os.sep+"qmlimportscanner"]
    #qmlscanner_args.append("-rootPath")
    #qmlscanner_args.append(path)

    # Alternative QML discovery (start)

    # bash command:
    # qmlfiles=$(find . | grep "\.qml$"); qmlimportscanner -qmlFiles $qmlfiles -importPath $QT_INSTALL_DIR/qml/)

    # Find all qml files in 'path'
    find = subprocess.Popen(('find', path), stdout=subprocess.PIPE)
    qml_files = ""
    try:
        qml_files = subprocess.check_output(('grep', "\.qml$"), stdin=find.stdout)
    except subprocess.CalledProcessError as e:
        warn("No QML files found OR grep might have failed")
        qml_files = "" #raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))

    find.wait()

    # Use Qt installed 'qmlimportscanner' to get list of dependant libraries
    qml_files = qml_files.split(b'\n')
    qmlscanner_args = [qt_bin_dir+os.sep+"qmlimportscanner", "-qmlFiles"]
    for qml_file in qml_files:
        qml_file = qml_file.decode("utf-8").strip()
        if qml_file == '':
            info("qml_file empty: "+qml_file)
            continue
        info("qml_file not empty: "+qml_file)
        qmlscanner_args.append(qml_file)
    # Alternative QML discovery (end)

    qmlscanner_args.append("-importPath")
    qmlscanner_args.append(lib_path)
    json_output = subprocess.check_output(qmlscanner_args)
    qml_imports = json.loads(json_output)

    # Clean qmlimportscanner output for unwanted entries
    qml_imports_whitelist = []
    for qml_import in qml_imports:
        if qml_import['type'] == "directory":
            continue
        if qml_import['type'] == "javascript": # TODO imports in javascript?
            continue

        if qml_import['type'] == "module":

            if not 'path' in qml_import:
                warn("Couldn't read 'path' property of import %s (%s). Skipping..." % (qml_import['name'],qml_import))
                continue

            qml_imports_whitelist.append(qml_import)

    # Locate *.so plugin files based on 'qmlimportscanner' output
    plugins = {}
    imports = {}
    for qml_import in qml_imports_whitelist:
        path = qml_import['path'].rstrip(os.sep)

        imports[qml_import['name']] = qml_import['path']

        if not 'plugin' in qml_import:
            warn("Couldn't read 'plugin' property of import %s (%s). Skipping..." % (qml_import['name'],qml_import))
            continue

        plugin = qml_import['plugin']
        full_path = path+os.sep+'lib'+plugin+'.so'
        realpath = os.path.realpath(full_path)
        relativePath = ""

        if 'relativePath' in qml_import:
            relativePath = qml_import['relativePath'].rstrip(os.sep)

        if not os.path.exists(realpath):
            warn("Can't find shared object file (%s) for %s (%s)" % (full_path, qml_import['name']+' '+qml_import['version'], plugin))
            continue

        debug("Resolved QML import '%s %s' to '%s'" % (qml_import['name'], qml_import['version'], realpath))

        so = os.path.basename(realpath)
        details = { 'so':so, 'path':full_path, 'realpath':realpath, 'relativePath': relativePath, 'dependants':set([ exe ]), 'type':'qml plugin' }
        plugins[so] = details

    return plugins,imports

def determine_qt_plugins(deps):

    plugin_list = set()

    for so in deps:
        
        soCPY = ""
        if type(so) == str:
            soCPY = so
        else: 
            soCPY = str(so, "utf-8")

        # Platform plugin
        if soCPY.startswith("libQt5Gui"):
            debug("'%s' found" % so)
            plugin_list.add('platforms'+os.sep+'libqxcb.so')

        # CUPS print support
        if soCPY.startswith("libQt5PrintSupport"):
            debug("'%s' found" % so)
            plugin_list.add('printsupport'+os.sep+'libcupsprintersupport.so')

        # SVG support
        if soCPY.startswith("libQt5Svg"):
            debug("'%s' found" % so)
            plugin_list.add('imageformats'+os.sep+'libqsvg.so')

        # Network support
        if soCPY.startswith("libQt5Network"):
            debug("'%s' found" % so)
            for plugin in os.listdir(qt_plugin_dir+os.sep+'bearer'):
                plugin_list.add('bearer'+os.sep+plugin)

        # SQL support
        if soCPY.startswith("libQt5Sql"):
            debug("'%s' found" % so)
            for plugin in os.listdir(qt_plugin_dir+os.sep+'sqldrivers'):
                plugin_list.add('sqldrivers'+os.sep+plugin)

        # Multimedia support
        if soCPY.startswith("libQt5Multimedia."):
            debug("'%s' found" % so)
            for plugin in os.listdir(qt_plugin_dir+os.sep+'mediaservice'):
                plugin_list.add('mediaservice'+os.sep+plugin)
            for plugin in os.listdir(qt_plugin_dir+os.sep+'audio'):
                plugin_list.add('audio'+os.sep+plugin)
            for plugin in os.listdir(qt_plugin_dir+os.sep+'playlistformats'):
                plugin_list.add('playlistformats'+os.sep+plugin)

        # Sensors support
        if soCPY.startswith("libQt5Sensors"):
            debug("'%s' found" % so)
            for plugin in os.listdir(qt_plugin_dir+os.sep+'sensors'):
                plugin_list.add('sensors'+os.sep+plugin)
            for plugin in os.listdir(qt_plugin_dir+os.sep+'sensorgestures'):
                plugin_list.add('sensorgestures'+os.sep+plugin)

        # Positioning support
        if soCPY.startswith("libQt5Positioning"):
            debug("'%s' found" % so)
            for plugin in os.listdir(qt_plugin_dir+os.sep+'position'):
                plugin_list.add('position'+os.sep+plugin)


    for image_plugins in os.listdir(qt_plugin_dir+os.sep+'imageformats'):
        if not image_plugins.startswith('libqsvg'):
            plugin_list.add('imageformats'+os.sep+image_plugins)

    if 'platforms'+os.sep+'libqxcb.so' in plugin_list:
        for plugin in os.listdir(qt_plugin_dir+os.sep+'xcbglintegrations'):
            plugin_list.add('xcbglintegrations'+os.sep+plugin)

    not_added = set()
    for root, subdirs, files in os.walk(qt_plugin_dir):
        for f in files:
            plugin = os.path.join(root,f).replace(qt_plugin_dir+os.sep,'')
            if plugin not in plugin_list:
                not_added.add(plugin)
        #print files #os.path.join(root, files)
        #
    not_added = list(not_added)
    debug("Left out these Qt plugins: %s" % not_added)

    return list(plugin_list), not_added


def strip(f):
    if os.path.isfile(f):
        res = subprocess.call(('strip', "-x", f))
        debug("Stripping '%s'" % f)
        if res > 0:
            warn("'strip' command failed with return code '%s' on file '%s'" % (res,f))
        return res
    return 0

def patch_elf(options,f):
    arguments = ['patchelf'] + options + [ f ]
    debug("Running patchelf '%s'" % arguments)
    res = subprocess.call(arguments)
    if res > 0:
        warn("'patchelf' command failed with return code '%s' on file '%s'" % (res,f))
    return res



def create_qt_conf(conf_path):

    # Set Plugins and imports paths
    qt_conf = "[Paths]\n"
    qt_conf += "Plugins = plugins\n"
    qt_conf += "Imports = qml\n"
    qt_conf += "Qml2Imports = qml\n";

    qc = conf_path+os.sep+"qt.conf"
    debug("Writing qt.conf to '%s'" % qc)
    text_file = open(qc, "w")
    text_file.write(qt_conf)
    text_file.close()

def create_desktop_file(path):
    d = "[Desktop Entry]\n"
    d += "Type=Application\n"
    d += "Name=Application\n"
    d += "Exec=AppRun %F\n"
    d += "Icon=default\n"
    d += "Comment=Edit this default file\n"
    d += "Terminal=true\n"

    f = path+os.sep+"default.desktop"
    debug("Writing desktop to '%s'" % f)
    text_file = open(f, "w")
    text_file.write(d)
    text_file.close()

    # Copy default appimage icon from script home
    icon_file = script_path+os.sep+'icon.png'
    if os.path.isfile(icon_file):
        shutil.copyfile(icon_file, path+os.sep+'default.png')
    else:
        debug("Fetching remote icon")
        os.system('wget -t 1 -T 5 --quiet https://raw.githubusercontent.com/Larpon/linuxdeployqt.py/master/icon.png -O '+path+os.sep+'default.png')

def build_appdir(dest_dir,executable,dependencies,qml_dirs,qt_plugins):

    from distutils.dir_util import copy_tree

    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)

    appdir_libs = 'lib'
    appdir_qml = 'qml'
    appdir_plugins = 'plugins'
    #appdir_libs = 'lib'

    if not os.path.exists(dest_dir+os.sep+appdir_libs):
        os.makedirs(dest_dir+os.sep+appdir_libs)
    if not os.path.exists(dest_dir+os.sep+appdir_plugins):
        os.makedirs(dest_dir+os.sep+appdir_plugins)
    if not os.path.exists(dest_dir+os.sep+appdir_qml):
        os.makedirs(dest_dir+os.sep+appdir_qml)

    dest_file = dest_dir+os.sep+os.path.basename(executable)

    # TODO overwrite option/awareness

    # Handle executable
    shutil.copyfile(executable,dest_file) # overrides dest

    # Strip executable
    strip(dest_file)
    patch_elf(["--set-rpath", "$ORIGIN"+os.sep+appdir_libs],dest_file)

    for dep in dependencies:
        details = dependencies[dep]

        depCPY = ""
        if type(dep) == str:
            depCPY = dep
        else: 
            depCPY = str(dep, "utf-8")

        src = details['realpath']

        srcCPY = ""
        if type(src) == str:
            srcCPY = src
        else: 
            srcCPY = str(src, "utf-8")

        if details['type'] == 'lib':

            dst = dest_dir+os.sep+appdir_libs+os.sep+depCPY
            debug("Copying library "+depCPY+": "+srcCPY+' -> '+dst)
            shutil.copyfile(srcCPY,dst) # overrides dest no questions asked
            strip(dst)

        elif details['type'] == 'qml plugin':
            # relativePath = details['relativePath']
            dst = dest_dir+os.sep+appdir_qml+os.sep+srcCPY.replace(qt_qml_dir+os.sep,'',1) #+relativePath+os.sep+details['so']
            if not os.path.exists(os.path.dirname(dst)):
                os.makedirs(os.path.dirname(dst))
            dst_dir = os.path.dirname(dst)
            src_dir = os.path.dirname(srcCPY)

            debug("Copying qml plugin dir "+depCPY+": "+src_dir+' -> '+dst_dir)
            copy_tree(src_dir, dst_dir,update=1)
            strip(dst)
        else:
            debug("Unhandled type '%s' (%s)" % (details['type'],))


    for qml_import in qml_dirs:

        src_dir = qml_dirs[qml_import]
        dst_dir = dest_dir+os.sep+appdir_qml+os.sep+src_dir.replace(qt_qml_dir+os.sep,'',1)

        debug("Copying Qt qml dir "+qml_import+": "+src_dir+' -> '+dst_dir)
        copy_tree(src_dir, dst_dir,update=1)

    for qt_plugin in qt_plugins:

        src = qt_plugin_dir+os.sep+qt_plugin
        dst = dest_dir+os.sep+appdir_plugins+os.sep+qt_plugin
        if not os.path.exists(os.path.dirname(dst)):
            os.makedirs(os.path.dirname(dst))

        debug("Copying Qt plugin "+os.path.basename(qt_plugin)+": "+src+' -> '+dst)
        shutil.copyfile(src,dst) # overrides dest no questions asked
        strip(dst)

    # Make qt.conf file
    create_qt_conf(dest_dir)

    # Make default desktop file
    create_desktop_file(dest_dir)

    # Make AppRun symlink
    os.system('cd "'+dest_dir+'" && ln -s '+os.path.basename(executable)+' AppRun')

    # Make AppRun executable
    os.system('cd "'+dest_dir+'" && chmod +x AppRun')


def build_appimage(appdir,appimage):
    debug("Building AppImage %s from %s" % (appimage,appdir))
    res = subprocess.call(('appimagetool', appdir, appimage))
    return res

def build_fake_qml(qml_import):

    tmp_qml = "import "+qml_import+"\nItem {\n}"

    tmp_file_path = os.path.join(tmp_dir,os.path.basename(tempfile.NamedTemporaryFile().name))+'.qml'
    with open(tmp_file_path, "w") as tmp_file:
        tmp_file.write(tmp_qml)


# Main
if __name__ == "__main__":

    info('Executable: '+exe)
    info('Qt install directory: '+qt_install_dir)
    info('Qt QML directory: '+qt_qml_dir)
    info('Qt bin directory: '+qt_bin_dir)
    info('Qt plugin directory: '+qt_plugin_dir)

    info('QML includes scan directories (recursive): %s' % qml_scan_dirs)

    # Sanity exe argument checks
    if not os.path.isfile(exe):
        error("%s doesn't exist" % exe)
    if which(exe) is None:
        error("%s is not an executable" % exe)

    # Generate manual import fakes
    if qml_manual_imports is not None:
        for qml_manual_import in qml_manual_imports:
            build_fake_qml(qml_manual_import)


    if not os.listdir(tmp_dir) == []:
        qml_scan_dirs.append(tmp_dir)
    #exit(1)

    dependencies = {}

    info("Resolving shared object dependencies for '%s'" % os.path.basename(exe))
    exedeps = resolve_dependencies(exe)

    dependencies = merge_dicts(dependencies,exedeps)

    info("Resolving libraries for QML imports from '%s'" % qml_scan_dirs)
    qml_plugins = {}
    qml_dirs = {}
    for qml_scan_dir in qml_scan_dirs:
        qml_plugins_m, qml_dirs_m = qml_imports(qml_scan_dir,qt_qml_dir)
        qml_plugins = merge_dicts(qml_plugins,qml_plugins_m)
        qml_dirs = merge_dicts(qml_dirs,qml_dirs_m)

    dependencies = merge_dicts(dependencies,qml_plugins)

    info("Resolving dependencies for %s QML import libraries" % len(qml_plugins))
    for qml_plugin in qml_plugins:
        qmlso = qml_plugins[qml_plugin]['realpath']

        debug("Resolving shared object dependencies for QML plugin '%s'" % os.path.basename(qmlso))
        qml_plugin_deps = resolve_dependencies(qmlso)

        for qml_plugin_dep in qml_plugin_deps:

            qml_plugin_deps[qml_plugin_dep]['dependants'].add(qmlso)

            #qml_plugins_deps[qml_plugin_dep] = qml_plugin_deps[qml_plugin_dep]

            if qml_plugin_dep not in dependencies:
                debug("Adding QML import shared object dependency '%s'" % qml_plugin_dep)
                dependencies[qml_plugin_dep] = qml_plugin_deps[qml_plugin_dep]


    # Determine what Qt plugins are used so far
    used_plugins, not_used_plugins = determine_qt_plugins(dependencies)

    # Resolve dependencies for detected Qt plugins
    info("Resolving dependencies for %s Qt plugins" % len(used_plugins))
    for plugin in used_plugins:
        qp = qt_plugin_dir+os.sep+plugin
        debug("Resolving shared object dependencies for Qt plugin '%s'" % os.path.basename(qt_plugin_dir+os.sep+plugin))
        qt_plugin_deps = resolve_dependencies(qp)
        for qt_plugin_dep in qt_plugin_deps:

            qt_plugin_deps[qt_plugin_dep]['dependants'].add(qp)

            if qt_plugin_dep not in dependencies:
                debug("Adding Qt plugin shared object dependency '%s'" % qt_plugin_dep)
                dependencies[qt_plugin_dep] = qt_plugin_deps[qt_plugin_dep]

    class SetEncoder(json.JSONEncoder):
        def default(self, obj):
            if isinstance(obj, set):
                return list(obj)
            return json.JSONEncoder.default(self, obj)

    if output_file != '':
        if output_file == "-":
            print(json.dumps(dependencies, indent=2,cls=SetEncoder))
        else:
            with open(output_file, 'w') as outfile:
                json.dump(dependencies, outfile, indent=2,cls=SetEncoder)
            info("Wrote %s dependencies to %s" % (len(dependencies), output_file))

    if do_build_app_dir:
        info("Building AppDir in '%s'" % app_dir)
        build_appdir(app_dir,exe,dependencies,qml_dirs,used_plugins)

    if do_build_app_image:
        info("Building AppImage '%s' from '%s'" % (app_image,app_dir))
        build_appimage(app_dir,app_image)
