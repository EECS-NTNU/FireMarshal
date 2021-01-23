import os
import subprocess as sp
import shutil
import logging
import string
import pathlib
import git
import doit
from .. import wlutil

# Note: All argument paths are expected to be absolute paths

# Some common directories for this module (all absolute paths)
br_dir = pathlib.Path(__file__).parent
overlay = br_dir / 'overlay'
br_image = br_dir / 'buildroot' / 'output' / 'images' / 'rootfs.ext2'

initdAutostart = string.Template("""#!/bin/sh

SYSLOGD_ARGS=-n
KLOGD_ARGS=-n

start() {
    if [ -s "/autostart.sh" ]; then
        echo "Launch autostart script..."
        echo "/autostart.sh $args"
        /autostart.sh $args
        ret=$$?
        if [ $$ret -eq 0 ]; then
            echo "Autostart script done!"
        else
            echo "Autostart script retured non-zero exit code ($$ret)"
        fi
    fi
}

case "$$1" in
  start)
  start
  ;;
  stop)
  #stop
  ;;
  restart|reload)
  start
  ;;
  *)
  echo "Usage: $$0 {start|stop|restart}"
  exit 1
esac

exit""")

def buildBuildRoot():
    wlutil.checkSubmodule(br_dir / 'buildroot')
    shutil.copy(br_dir / 'buildroot-config', br_dir / 'buildroot' / '.config')
    wlutil.run(['make'], cwd=br_dir / 'buildroot')

class Builder:
    def baseConfig(self):
        return {
                'name' : 'br_ntnu',
                'distro' : 'br_ntnu',
                'workdir' : br_dir,
                'builder' : self,
                'img' : str(br_image)
                }

    # Build a base image in the requested format and return an absolute path to that image
    def buildBaseImage(self):
        """Ensures that the image file specified by baseConfig() exists and is up to date.

        This is called as a doit task.
        """
        try:
            buildBuildRoot()
        except wlutil.SubmoduleError as e:
            return doit.exceptions.TaskFailed(e)

    def fileDeps(self):
        # List all files that should be checked to determine if BR is uptodate
        deps = []
        deps.append(br_dir / 'buildroot-config')
        deps.append(pathlib.Path(__file__))

        return deps

    # Return True if the base image is up to date, or False if it needs to be
    # rebuilt. This is in addition to the files in fileDeps()
    def upToDate(self):
        return [wlutil.config_changed(wlutil.checkGitStatus(br_dir / 'buildroot'))]

    # Set up the image such that, when run in qemu, it will run the script "script"
    # If None is passed for script, any existing bootscript will be deleted
    @staticmethod
    def generateBootScriptOverlay(script, args):
        # How this works:
        # The buildroot repo has a pre-built overlay with a custom S99run
        # script that init will run last. This script will run the /firesim.sh
        # script at boot. We just overwrite this script.

        # Disable superfluous runscript that does nothing
        if script is not None and hasattr(script, "name") and script.name == 'null_run.sh':
            script = None

        scriptDst = overlay / 'autostart.sh'
        if script != None:
            shutil.copy(script, scriptDst)
        else:
            # Create a blank init script because overlays won't let us delete stuff
            # Alternatively: we could consider replacing the default.target
            # symlink to disable the firesim target entirely
            scriptDst.unlink()
            scriptDst.touch()
        
        scriptDst.chmod(0o755)

        with open(overlay / 'etc/init.d/S99autostart', 'w') as f:
            f.write(initdAutostart.substitute(args='' if args is None else ' '.join(args)))
        
        return overlay
