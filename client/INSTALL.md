## Looking Glass Client

This guide will step you through building the looking glass client from
source, before you attempt to do this you should have a basic
understanding of how to use the shell.

### Building the Application

-----

#### Installing Build Dependencies

These required libraries and tools should be installed first.

##### Required Dependencies

  - cmake
  - gcc | clang
  - fonts-freefont-ttf
  - libegl-dev
  - libgl-dev
  - libfontconfig1-dev
  - libgmp-dev
  - libsdl2-dev
  - libsdl2-ttf-dev
  - libspice-protocol-dev
  - make
  - nettle-dev
  - pkg-config

###### May be disabled

These dependencies are required by default, but may be omitted if their
feature is disabled when running [cmake](#Building).

  - Disable with `cmake -DENABLE_BACKTRACE=no`
      - binutils-dev
  - Disable with `cmake -DENABLE_X11=no`
      - libx11-dev
      - libxfixes-dev
      - libxi-dev
      - libxss-dev
  - Disable with `cmake -DENABLE_WAYLAND=no`
      - libwayland-bin
      - libwayland-dev
      - wayland-protocols

##### Debian (and maybe Ubuntu)

``` bash
apt-get install binutils-dev cmake fonts-freefont-ttf libfontconfig1-dev libsdl2-dev libsdl2-ttf-dev libspice-protocol-dev libx11-dev nettle-dev wayland-protocols
```

##### Fedora 29+

``` bash
dnf install binutils-devel cmake texlive-gnu-freefont fontconfig-devel SDL2-devel SDL2_ttf-devel spice-protocol libX11-devel nettle-devel wayland-protocols-devel \
            gcc libXScrnSaver-devel libXfixes-devel libXi-devel wayland-devel
```

##### OpenSuSE Leap 15.0+

``` bash
zypper install binutils-devel make cmake fontconfig-devel libSDL2-devel libSDL2_ttf-devel spice-protocol-devel libX11-devel libnettle-devel wayland-protocols-devel \
               libconfig-devel libXi-devel libXss-devel libwayland-egl-devel nettle
```

##### Arch Linux / Manjaro

``` bash
pacman -Syu binutils cmake gnu-free-fonts fontconfig sdl2 sdl2_ttf spice-protocol libx11 nettle \
            gcc make pkgconf glu
```

##### Void Linux

``` bash
xbps-install -Syu binutils-devel cmake freefont-ttf fontconfig-devel SDL2-devel SDL2_ttf-devel spice-protocol libX11-devel nettle-devel \
                  gcc make pkg-config
```

##### Gentoo

First set up the necessary USE flags if needed:

``` bash
echo "media-libs/libsdl2 gles2" >> /etc/portage/package.use/libsdl2
echo "media-libs/nettle gmp" >> /etc/portage/package.use/nettle
```

Then fetch the packages:

``` bash
emerge sys-devel/binutils dev-util/cmake media-fonts/freefonts media-libs/libsdl2 media-libs/sdl2-ttf app-emulation/spice-protocol \
       media-libs/fontconfig dev-libs/nettle \
       media-libs/glu
```

This list can also be placed into a setfile in /etc/portage/sets/ so
that they can be updated with

``` bash
emerge @setfile
```

#### Downloading

Either visit the site at [Looking Glass Download
Page](https://looking-glass.hostfission.com/downloads)

Or pull the lastest **bleeding-edge version** using the **git** command.

**Note: If you are using the latest bleeding-edge from the master branch
you MUST download/use the corresponding host application**

``` bash
git clone --recursive https://github.com/gnif/LookingGlass.git
```

#### Building

If you downloaded the file via the web link then you should have a 'zip'
file. Simply unzip and cd into the new directory. If you used 'git' then
cd into the 'LookingGlass' directory.

``` bash
mkdir client/build
cd client/build
cmake ../
make
```

  - NOTE: The most common compile error is related to backtrace support.
    This can be disabled by adding the following option to the cmake
    command: **-DENABLE\_BACKTRACE=0**, however, if you disable this and
    need support for a crash please be sure to use gdb to obtain a
    backtrace manually or there is nothing that can be done to help you.

Should this all go well you should be left with the file
**looking-glass-client**. Before you run the client you will first need
to configure either Libvirt or Qemu (whichever you prefer) and then set
up the Windows side service.

You can call the client from the build directory; or, you can make it
callable generally by adding the directory to your path or issuing

``` bash
ln -s $(pwd)/looking-glass-client /usr/local/bin/
```

from the build directory.

### libvirt Configuration

-----

This article assumes you already have a fully functional libvirt VM with
PCI Passthrough working on a dedicated monitor. If you do not please
ensure this is configured before you proceed.

If you use virt-manager, this guide also applies to you, since it uses
libvirt.

**If you are using QEMU directly, this does not apply to you.**

Add the following to the libvirt machine configuration inside the
'devices' section by running "virsh edit VM" where VM is the name of
your virtual machine.

``` xml
<shmem name='looking-glass'>
  <model type='ivshmem-plain'/>
  <size unit='M'>32</size>
</shmem>
```

The memory size (show as 32 in the example above) may need to be
adjusted as per [Determining Memory](#Determining_Memory)
section.

#### Spice Server

If you would like to use Spice to give you keyboard and mouse input
along with clipboard sync support, make sure you have a
<graphics type='spice'> device, then:

  - Find your <code>
    <video>
    </code> device, and set <model type='none'>
      - If you can't find it, make sure you have a <graphics> device,
        save and edit again
      - On older libvirt versions, just disable the device in Windows
        Device Manager
  - Remove the <input type='tablet'/> device, if you have one
  - Create an <input type='mouse'/> device, if you don't already have
    one
  - Create an <input type='keyboard' bus='virtio'/> device to improve
    keyboard usage
      - This requires the *vioinput* driver from
        [virtio-win](https://fedorapeople.org/groups/virt/virtio-win/direct-downloads/stable-virtio/)
        to be installed in the guest

If you want clipboard synchronization please see [FAQ\#How to enable
clipboard synchronization via
SPICE](FAQ.md#How_to_enable_clipboard_synchronization_via_SPICE)

#### AppArmor

For libvirt versions before **5.10.0**, if you are using AppArmor, you
need to add permissions for QEMU to access the shared memory file. This
can be done by adding the following to
*/etc/apparmor.d/abstractions/libvirt-qemu*.

`/dev/shm/looking-glass rw,`

### Qemu Commands

-----

**If you are using virt manager/libvirt then this does not apply to
you.**

Add the following to the commands to your QEMU command line, adjusting
the bus to suit your particular configuration:

``` bash
-device ivshmem-plain,memdev=ivshmem,bus=pcie.0 \
-object memory-backend-file,id=ivshmem,share=on,mem-path=/dev/shm/looking-glass,size=32M
```

The memory size (show as 32 in the example above) may need to be
adjusted as per [Determining Memory](#Determining_Memory)
section.

### Determining Memory

-----

You will need to adjust the memory size to a value that is suitable for
your desired maximum resolution using the following formula:

`width x height x 4 x 2 = total bytes` `total bytes / 1024 / 1024 =
total megabytes + 2`

For example, for a resolution of 1920x1080 (1080p)

`1920 x 1080 x 4 x 2 = 16,588,800 bytes` `16,588,800 / 1024 / 1024
= 15.82 MB + 2 = 17.82`

You must round this value up to the nearest power of two, which with the
above example would be 32MB

It is suggested that you create the shared memory file before starting
the VM with the appropriate permissions for your system, this only needs
to be done once at boot time, for example (this is a sample script only,
do not use this without altering it for your requirements):

`touch /dev/shm/looking-glass && chown user:kvm /dev/shm/looking-glass
&& chmod 660 /dev/shm/looking-glass`

## Looking Glass Service (Windows)

You must first run the Windows VM with the changes noted above in either
the [libvirt](#libvirt_Configuration) or
[Qemu](#Qemu_Commands) sections.

### Installing the IVSHMEM Driver

-----

Windows will not prompt for a driver for the IVSHMEM device, instead, it
will use a default null (do nothing) driver for the device. To install
the IVSHMEM driver you will need to go into the device manager and
update the driver for the device "PCI standard RAM Controller" under the
"System Devices" node.

**French:** "Gestionnaire de périphérique" -\> "Périphériques Système"
-\> "Contrôleur de RAM Standard PCI"

**English:** "Device Manager" -\> "System Devices" -\> "PCI standard RAM
Controller"

A signed Windows 10 driver can be obtained from Red Hat for this device
from the below address:

<https://fedorapeople.org/groups/virt/virtio-win/direct-downloads/upstream-virtio/>

Please note that you must obtain version 0.1.161 or later

#### A note about IVSHMEM and Scream Audio

Using IVSHMEM with Scream may interfere with Looking Glass as it may try
to use the same device. Please do not use the IVSHMEM plugin for Scream.
Use the default network transfer method. The IVSHMEM method induces
additional latency that is built into its implementation. When using
VirtIO for a network device the VM is already using a highly optimized
memory copy anyway so there is no need to make another one.

If you insist on using IVSHMEM for Scream despite its inferiority to the
default network implementation the Windows Host Application can be told
what device to use. Create a looking-glass-host.ini file in the same
directory as the looking-glass-host.exe file. In it, you can use the
os:shmDevice option like so:

``` INI
[os]
shmDevice=1
```

### Using the Windows Host Application

-----

Start by downloading the correct version for your release from
<https://looking-glass.io/downloads>. You can either choose between
**Official Releases** which is is stable or **Release Candidates** that
tries to be stable but has new features. **Note:** If your
**looking-glass-client** was created by building from the **master
branch** you have to pick the **Bleeding Edge** version.

Next, extract the zip archive. Then, run the
"looking-glass-host-setup.exe" installer and click through it. By
default, the installer will install a service that automatically starts
the host application at boot. The installer can also be installed in
silent mode with the "/S" switch. Other command line options for the
installer are documented by running it with the "/h" switch. There is
also an unofficial Chocolatey package available, install with "choco
install looking-glass-host --pre"

The windows host application captures the windows desktop and stuffs the
frames into the shared memory via the shared memory virtual device,
without this Looking Glass will not function. It is critical that the
version of the host application matches the version of the client
application, as differing versions can be, and usually are,
incompatible.

**Note:** As of 2020-10-23, Microsoft Defender is known to mark the
Looking-Glass host executable as a virus and in some cases will
automatically delete the file.

**Note:** As of 2020-08-12 (commit
dc4d1d49fac2361e60c9bb440bc88ce05f6c1cbd), the below instructions are
deprecated. The host application now has an installer that installs a
system-wide service to run the Looking Glass host application. When
upgrading please be sure to remove the scheduled task if you have
already created one.

<div class="toccolours mw-collapsible mw-collapsed">

Instructions for versions prior to 2020-08-12.

<div class="mw-collapsible-content">

To get the Windows-Host-Application running after restart you need to
run it as a privileged task we do that by starting **cmd.exe** as
**administrator** and running a command in it which creates a windows
task.

**Note:** At this time the Looking Glass host does not support running
under unprivileged users. Your user account must have administrator
privileges for the windows task to function.

``` batch
SCHTASKS /Create /TN "Looking Glass" /SC  ONLOGON /RL HIGHEST /TR C:\Users\<YourUserName>\<YourPath>\looking-glass-host.exe
```

Copy the following command in to your cmd shell and replace the
'''<YourUserName> ''' with your username **(e.g. "games")** and your
'''<YourPath> ''' with the part where the looking-glass-host.exe is
stored **(e.g. "Documents")** .

![Screenshot\_cmd\_windowstask.png](Screenshot_cmd_windowstask.png
"Screenshot_cmd_windowstask.png")

Now you simply need to hit enter in to the cmd shell and restart the vm
to test if it worked.

</div>

</div>

## Running the Client

The client command is the binary file: **looking-glass-client**. This
command should run after the Windows Host Application has started.

For an updated list of arguments visit:
<https://github.com/gnif/LookingGlass/blob/master/client/README.md>

Common options include '-s' for disabling spice, '-S' for disabling the
screen saver, and '-F' to automatically enter full screen.
