NTNU - FireMarshal
==================================

This is a fork from FireMarshal containing image specifications that we require to execute our workloads in FireSim. It contains also some fixes and additions of the wlutil scripts that were found to be useful (better filesystem repair, umount retries...). To get started you need to understand the structure and requirements to build linux images.

# Requirements
* >= QEMU emulator version 5.2.50 (v5.2.0-1340-ge81eb5e6d1)
* >= gcc-9/gcc-10
* >= 9.2/10.1 riscv64-unknown-linux-gnu toolchain

By default the build process will require a gcc-10 toolchain for host and riscv64. Switch the `buildroot-config` under `boards/firechip/base-workloads/ntnu-base/` to the desired configuration for gcc-9 or gcc-10:
```
rm buildroot-config
# GCC-9
ln -s buildroot-config-gcc-9 buildroot-config
# GCC-10 (default)
ln -s buildroot-config-gcc-10 buildroot-config
```

Ubuntu Requirements to build kernel:  
```sudo apt lzop libncurses-dev flex bison openssl libssl-dev dkms libelf-dev libudev-dev libpci-dev libiberty-dev autoconf```

This fork does not try to autodetect your toolchain and assumes gcc-10 or higher. It should be easy however to adjust to lower versioned toolchains if necessary. It also uses fixed configuration files for busybox and buildroot and does not try to dynamically patch anything.

# Getting Started

**Note:** Images are building up on each other, but there is no need to manually build them all. For exmaple if you build a spec2017-ref image it will automatically build the parent images ntnu-base and spec2017.

To login to any image use these credentials:
* **Username:** root
* **Password:** ntnu

### br\_ntnu
This is the buildroot base image which is located under `wlutil/br_ntnu/` together with its configuration and specification to build. This image contains the basic logic on how to autostart workloads and commands up on boot. There should be no need to touch or change this image except when you miss a library or basic tool that buildroot/busybox can supply on its own.

### ntnu-base
This is the base image which is supposed to contain all our tools that are required in the simulation. Currently it contains perf, pperf, invoke.py. It is located under `boards/firechip/base-workloads/ntnu-base`. When build the first time it will populate the overlay directory with our tools which are checked out from sub repositories. If you want to rebuild e.g. perf you have to delete the created overlay directory `overlay/usr/local`, which forces a rebuild.

This image will provide a very basic linux image containing no benchmarks. You can boot this up, login and experiment.

Build this image with `./marshal -v build ntnu-base.json`

### spec2017

This is the base spec2017 image containing **all** benchmarks and input sets together with specification how they are executed designed invoke.py. To build this image you have to supply [spec2017.tar.zst (NTNU login required)](https://studntnu-my.sharepoint.com/:f:/g/personal/bjorngot_ntnu_no/EtgaWv3ni1RNho80h9m9hIABuYKpn_vUmQ72GTMG1cug_g?e=Peg9km) archive at `boards/firechip/base-workloads/spec2017`.

This image will provide the basic linux image together with all spec benchmarks. Boot this up and launch any benchmark with invoke.py in `/opt/spec2017/`.

Build this image with `./marshal -v build spec2070.json`

### parsec

This is the base parsec image containing **all** benchmarks and input sets together with specification how they are executed designed invoke.py. To build this image you have to supply [parsec.tar.zst (NTNU login required)](https://studntnu-my.sharepoint.com/:f:/g/personal/bjorngot_ntnu_no/EtgaWv3ni1RNho80h9m9hIABuYKpn_vUmQ72GTMG1cug_g?e=Peg9km) archive at `boards/firechip/base-workloads/parsec`.

This image will provide the basic linux image together with all spec benchmarks. Boot this up and launch any benchmark with invoke.py in `/opt/parsec/`.

Build this image with `./marshal -v build parsec.json`

### spec2017-ref/train/test

This image will provide the actual benchmark jobs for firesim. It will make a copy of the base spec2017 image and prune it down to the necessary files. It will also create one image **per** benchmark.

After building you will have one image e.g. spec2017-ref containing all benchmarks with the ref input set as well as one image per benchmark and its input files e.g. spec2017-ref-500.perlbench\_r.

Build this image with `./marshal -v build workloads/spec2017-ref.json` where ref can also be train or test for the respective input sets.

### parsec-native/simdev/simlarge/simmedium/simsmall/test

This image will provide the actual benchmark jobs for firesim. It will make a copy of the base parsec image and prune it down to the necessary files. It will also create one image **per** benchmark.

After building you will have one image e.g. parsec-native containing all benchmarks with the native input set as well as one image per benchmark and its input files e.g. parsec-native-blackscholes.

Build this image with `./marshal -v build workloads/parsec-native.json` where native can also be simdec, simlarge, simmedium, simsmall or test for the respective input sets.

**Note:** there is also a parsec.serial-native/simdev/simlarge/simmedium/simsmall/test target to build the images of the parsec serial benchmarks.

FireMarshal
==================================

This tool builds base images for several linux-based distros that work with qemu,
spike, and firesim.

This is just a quick primer. To see full documentation, please see the official
documentation:
https://firemarshal.readthedocs.io/en/latest/index.html

# Requirements
The easiest way to use Marshal is to run it via Chipyard
(https://chipyard.readthedocs.io/en/latest/) or FireSim
(https://docs.fires.im/en/latest/). However, this is not required. To run
FireMarshal independently, you will need the following dependencies:

## Standard Packages
centos-requirements.txt is a list of packages for centos7 that are needed by
marshal. You can install these with:
```
cat centos-requirements.txt | sudo xargs yum install -y
```

Package names may be different on other distributions.

### Note for Ubuntu
The libguestfs-tools package (needed for the guestmount command) does not work
out of the box on Ubuntu. See
https://github.com/firesim/firesim-software/issues/30 for a workaround.

## Python
This project was written for python 3.6. You can install all dependencies using:
```
pip3 install -r python-requirements.txt
```

## RISC-V Tools
In addition to standard libraries, you will need a RISC-V compatible toolchain,
the RISC-V isa simulator (spike).

See the [Chipyard documentation](https://chipyard.readthedocs.io/en/latest/Chipyard-Basics/Initial-Repo-Setup.html#building-a-toolchain)
for help setting up a known-good toolchain and environment.

## Qemu
Qemu is the default simulator used by firemarshal. We require version v5.0.0 or
greater. If you aren't using chipyard, you can get it from:

https://github.com/qemu/qemu/tree/v5.0.0

# Basic Usage
If you only want to build bare-metal workloads, you can skip updating
submodules. Otherwise, you should update the required submodules by running:

    ./init-submodules.sh

Building workloads:

    ./marshal build br-base.json

To run in qemu:

    ./marshal launch br-base.json

To install into FireSim (assuming you cloned this as a submodule of firesim or chipyard):

    ./marshal install br-base.json

# Security Note
Be advised that FireMarshal will run initialization scripts provided by
workloads. These scripts will have all the permissions your user has, be sure
to read all workloads carefully before building them.

# Getting Help / Discussion:
* For general questions, help, and discussion: use the FireSim user forum: https://groups.google.com/forum/#!forum/firesim
* For bugs and feature requests: use the github issue tracker: https://github.com/firesim/FireMarshal/issues
* See CONTRIBUTING.md for more details
