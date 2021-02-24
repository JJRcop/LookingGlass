# Frequently Asked Questions

## General

### How does Looking Glass work?

Please see the following video that covers this:

<https://www.youtube.com/watch?v=U44lihtNVVM>

### Can I feed the VM directly into OBS?

Yes provided you're using a bleeding-edge version, [OBS](https://looking-glass.io/wiki/OBS)
support is brand new. There is a plugin to build in the \`obs\`
directory for this.

**Note**: This plugin is brand new and is missing mouse cursor support.

**Update**: As of commit 4f40ce4b40dbf98b3cacd1baaf66fc11f8633c0c, full
cursor support is now available and is slated for Beta 3.

### Why is my UPS (Updates Per Second) so low?

There are several reasons why this can happen, the most common is your
capture resolution. At the time of this writing, the windows capture
methods struggle to capture high resolutions under certain
circumstances, as a general rule of thumb anything greater than 1200p
(1920x1200) starts to suffer from performance issues.

Another cause can be how the game or application you are running is
configured. Because of the way windows integrate with the WDM (Windows
Desktop Manager) running applications in "Full Screen" mode may in some
cases cause a large performance penalty. Try switching to windowed
full-screen mode, the difference in performance can be like night and
day. Titles known to be affected by this are Fallout76, The BattleField
Series and SCUM.

Some titles do some strange things at early initialization that cause
capture performance issues. One such title is the Unigine Valley
benchmark where the capture rate is limited to 1/2 the actual rate. For
an unknown reason to both myself and the Unigine developers a simple
task switch (alt+tab) in and out resolves the issue. This is not a
Looking Glass bug.

### Is my GPU supported?

Your guest GPU almost certainly supports DXGI. Use DxDiag to confirm
that you have support for WDDM 1.2 or greater.

The server-side (guest) probing error "Capture is not possible,
unsupported device or driver" indicates NVidia duplication has failed,
not that DXGI has failed. You can make the error go away by specifying
"-c DXGI"

### Why do I need Spice if I don't want a Spice display device?

You don't need Display Spice enabled. Looking Glass has a Spice client
built in to provide some conveniences, but you can disable it with the
"-s" argument.

Note that without Spice, you will not be sending mouse/keyboard events
to the guest nor will you get clipboard Synchronization support.

### Where is the host application for Linux?

The "Windows host application" is actually the display server, which
runs in the guest VM. The only thing that needs to run in your Linux
host OS is the \`looking-glass-client\` application

## Mouse

### The mouse is jumpy, slow, laggy when using SPICE

Please be sure to install the SPICE guest tools from
<https://www.spice-space.org/download.html#windows-binaries>.

### How to enable clipboard synchronization via SPICE

Install the SPICE guest tools from
<https://www.spice-space.org/download.html#windows-binaries>.

Choose only one of the settings below (the one that applies to your
configuration):

  - QEMU

<!-- end list -->

``` bash
-device virtio-serial-pci \
-chardev spicevmc,id=vdagent,name=vdagent \
-device virtserialport,chardev=vdagent,name=com.redhat.spice.0
```

  - Virt Manager GUI
      - Open Machine Properties \> Add Hardware \> Channel
      - Set Name to `com.redhat.spice.0` and Device Type to `Spice agent
        (spicevmc)` and click Finish
      - No need to add VirtIO Serial device, it will be added
        automatically

<!-- end list -->

  - libvirt using virsh
      - Edit the VM using virsh `sudo virsh edit VIRTUAL_MACHINE_NAME`
      - Paste the code from below right above </devices> (note the
        closing tag)

<!-- end list -->

``` xml
  <!-- No need to add VirtIO Serial device, it will be added automatically -->
  <channel type="spicevmc">
    <target type="virtio" name="com.redhat.spice.0"/>
    <address type="virtio-serial" controller="0" bus="0" port="1"/>
  </channel>
```

### The mouse doesn't stay aligned with the host.

This is due to windows mouse acceleration, it can be disabled by
following one of these methods:

  - Disabling pointer precision (Control Panel \> Mouse \> Pointer
    Options \> Uncheck Enhance pointer precision)
  - By changing the acceleration behavior with the following registry
    magic:
    <http://donewmouseaccel.blogspot.com.au/2010/03/markc-windows-7-mouse-acceleration-fix.html>
    (Contrary to the title this works just fine on Windows 10)

Some other things to try:

  - Have you tried pressing Scroll Lock to toggle mouse capture mode?
  - Try keeping the client window the same size as the guest's
    resolution.

### The cursor position doesn't update until I click

Make sure you have removed the Virtual Tablet Device from the Virtual
Machine. Due to the design of windows absolute pointing devices break
applications/games that require cursor capture and as such Looking Glass
does not support them.

## Audio

Looking Glass does not handle audio at all, nor will it ever. However an
audio solution know as Scream is included in the Additional Guides
section with two methods for installation:

  - [ Using Scream over LAN](https://looking-glass.io/wiki/Using_Scream_over_LAN)
  - [ Using Scream over IVSHMEM](https://looking-glass.io/wiki/Using_Scream_over_IVSHMEM)

Of the two **Scream over LAN** is objectively better however if complete
isolation of the VM from the Host network is necessary **Scream over
IVSHMEM** will get it done.

## Windows

### The Windows Logon Screen, Shutdown, UAC Dialogs, Ctrl+Alt+Del, LockScreen do not work.

<div class='toccolours mw-collapsible mw-collapsed'>

**Note:** This information is deprecated as of 2020-08-12, the host now
has a helper service to launch it at login with elevated privileges. As
of 2021-01-16, even the login screen can be captured.

<div class='mw-collapsible-content'>

These screens are on the "Secure Desktop", Microsoft has intentionally
limited access to capture this screen to prevent malware attempting to
capture information from these screens that might be sensitive. There is
little we can do here unless Microsoft is willing to let us sign the
host application as an 'Accessibility' program.\`

The workaround for this issue is to turn down the security level in the
\`User Account Control Settings\` dialog as shown here:

![Uac.png](Uac.png "Uac.png")

Users of NVIDIA cards that support the NVidia Capture API (Quadro,
Tesla, etc) will find this limitation only applies to the Windows Logon
Screen and Shutdown Screens, the NvFBC capture API is able to capture
the "Secure Desktop" outside of Microsoft's control.

</div>

</div>

### The Windows host program doesn't work as a service

<div class='toccolours mw-collapsible mw-collapsed'>

**Note:** This information is deprecated as of 2020-08-12, the host now
has a helper service to launch it at login with elevated privileges.

<div class='mw-collapsible-content'>

As of Windows Vista, Microsoft deprecated the ability for services to
interact with the user's desktop, and as of Windows 10 this is enforced.
There is nothing we can do about this.

Please see [Using the Windows Host
Application](INSTALL.md#Using_the_Windows_Host_Application)
for how to setup a scheduled task to launch the application at login.

</div>

</div>

### NvFBC (NVIDIA Capture API) doesn't work

Unfortunately, NVIDIA deemed fit to only allow owners of "Professional
Cards", and their own software (ShadowPlay) the right to use the NVIDIA
Capture API. There is little we can do about this other than to complain
to NVIDIA.

### The screen stops updating when left idle for a time

Windows is likely turning off the display to save power, you can prevent
this by adjusting the \`Power Options\` in the control panel.
