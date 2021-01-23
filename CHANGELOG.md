# Changelog

This changelog follows the format defined here: https://keepachangelog.com/en/1.0.0/
Versioning follows semantic versioning as described here: https://semver.org/spec/v2.0.0.html


# 2020-07-30
This is the initial commit of the NTNU fork

### Added
* ntnu-base image based on a recent buildroot and **recent toolchain** (gcc-10)
  includes perf, pperf and invoke.py tools and stress-ng

### Changed
* wlutil.py reads the gcc version now properly from the version string
* wlutil.py if e2fsck fails, it tries to repair it again with '-y' option before
  completely failing to repair e.g. broken symbolic links which are caused by
  improper filesystem cleanup through ext2/ext4 mount/unmount in qemu


## [1.9.0] - 2021-05-21
This is largely a maintenance release with a few minor features and a bunch of
bug fixes. The most significant change is a bump to Linux 5.7rc3. The new
'firesim-dir' configuration option is also signficant because it enables more
flexible deployment of FireMarshal in Chipyard and FireSim and is required for
Chipyard 1.3.0.

### Added
* A number of new global configuration options were added. This includes
  specifying where the 'install' command finds firesim (PR #127) and allowing
  for custom workload searchpaths (PR #140)
* PR #129 adds a number of new features to help support profiling and debug. In
  particular, Marshal now saves the raw kernel ELF file including debug symbols
  (BBL strips DWARF info). It also handles a few handy scripts for running
  FirePerf tools.
* PR #141 adds the 'post-bin' script option to workloads. This is primarily
  useful for building custom drivers but may have other uses.

### Changed
* PR #139 enables RVC in the kernel by default
* PR #143 changes the behavior of kernel fragments. Previously, workload kfrags
  were applied directly on top of the distro's default config. Now kfrags are
  inherited through the chain of parents.
* PR #151 bumps us to Linux 5.7rc3
* PR #148 switches to the SBI serial driver from the sifive uart due to a
  baud-rate configuration issue. This will be reversed eventually.

### Fixed
* PR #126 adds detection of changes in overlay directories that were missed before
* PR #128 fixes an issue when calling FireMarshal inside a makefile
* PR #133 fixes the --workdir option that had broken (and adds tests so it won't happen again)
* PR #135 jobs were not including device drivers

## [1.8.0] - 2020-01-24
This release introduces user-configurable options for FireMarshal through a
config file or environment variables. It also relaxes constraints on the
location of the workload directories (they can be anywhere in your filesystem
now). These changes make it much easier to add the marshal command to your PATH
and use it like a general utility.

### Added
* PR #81 Allows for multiple workload directories, and for those directories to be anywhere in the filesystem.
* PR #93 Enables basic functionality without initializing all submodules. Submodules are only accessed if they are actually used. Also introduces the init-submodules.sh script.
* PR #94 Adds the 'rootfs-size' option to allow for custom rootfs sizes. Also defaults to a tight-packing of the rootfs by default, greatly reducing default image sizes.
* PR #96 Adds options for machine configuration at launch time (memory and cpu amounts)
* PR #107 Adds a configuration system for common tool options (like default output locations or search paths). This will enable much more flexibility in the future.
* PR #116 and #117 allow for more customization in build components (BBL and Qemu sources, respectively)

### Changed
* PR #83 changes the way buildroot dependencies are checked. We now only rebuild buildroot if the submodule version changes (or if it's dirty) or if the buildroot configuration changes. This drastically reduces build times.
* PR #84 cleans up much of the logging output for the 'test' command
* PR #118 changes the default location of the build database (doit.db) and allows it to be configured.

### Fixed
* PR #101 switches to a github mirror for busybox from their private repo which was experiencing frequent server failures.
* Many documentation fixes and updates (including centos-requirements.txt, contributing guidelines, and tutorials). In particular, PR #121 expands the tutorial section.
 
## [1.7.0] - 2019-10-12
This release primarily updates several FireMarshal defaults to recent
upstreamed versions, notably: Linux (v5.3), Fedora (20190126), and Buildroot (2019.08).

Also, offical documentation has moved to: https://firemarshal.readthedocs.io/en/latest/

### Added
* PR #47 enables custom arguments to spike and qemu (see also PR #48 that adds an example RoCC accelerator workload that uses these arguments)
* PR #61 includes an example sha3 workload (matches example RTL in chipyard)

### Changed
* PR #40 Enables alternative package manager mirrors in Fedora (if the default mirrors go down)
* PR #41 Switches the default Fedora image from a nightly build to a stable release.
* PR #44 Enables compressed riscv instructions (RVC) by default in Linux
* PR #46 Updates the default linux kernel to (mostly) upstream v5.3 from the previous non-standard fork of Linux
  * Introduces an initramfs by default in all builds that install drivers for iceblk and icenic
  * Workloads now use Linux kernel configuration fragments instead of full linux configurations (enables easy bumping of kernel versions)
* PR #50 updates buildroot to 2019.08 and switches to the github mirror
* PR #51 Moves marshal documentation to this repository (from FireSim). Docs now hosted at https://firemarshal.readthedocs.io/en/latest/.

### Fixed
* PR #57 fixes a bug where host-init scripts may not run in parent workloads unless they were built explicitly

## [1.6.0] - 2019-06-25
This release introduces a number of bug-fixes and support for standalone behavior (marshal can now be cloned by itself on a machine without sudo). It also synchronizes with firesim release 1.6.

### Added
* PR #31 allows firesim-software to work outside of a firesim environment without the need for sudo (just riscv-tools, qemu, and a few package requirements).
* PR #24 adds support for parallel 'launch' or 'test' commands.

### Changed
* PR #25 updates buildroot to a more recent version in order to support a more recent version of riscv-tools (as needed by firesim).
    * PR #32 finalizes the riscv-tools at gcc 7.2
* PR #22 enables multiple repository mirrors in the default fedora image

### Fixed
* PR #34 resolves #33 
* PR #29 fixes a number of issues with the full_test.sh script (bug #26 )
* PR #28 fixes an incompatibility with python36
    * Note that #31 officially updated the python version to 3.6, but firesim de-facto updated it in an earlier release.
