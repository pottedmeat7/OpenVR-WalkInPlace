![language](https://img.shields.io/badge/Language-C%2B%2B11-green.svg)  ![dependencies](https://img.shields.io/badge/Dependencies-Boost%201.63-green.svg)  ![license_gpl3](https://img.shields.io/badge/License-GPL%203.0-green.svg)

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](4X5XRV754K49Y)
# OpenVR-WalkInPlace

An OpenVR driver that applies virtual movement using a pedometer

The OpenVR driver hooks into the HTC Vive lighthouse driver and tracks movement of the HMD and Vive Controllers. It then applies movement into the virtual envrionment.

# Step Detection Motivation
I have done work with step detection locomotion with mobile phone VR games. So once I got my vive I was also interested in the options. Once I saw the functionality of OpenVR and https://github.com/matzman666/OpenVR-InputEmulator to emulate input and track poses I figured it has the ability to detect and "emulate" a step. In the long run I hope to expand this driver for many games and be able to use WIP locomotion as a standard input method.


# Current Games that Work Best with OpenVR-WalkInPlace

- Arizona Sunshine
- Onward
- Gorn
- Doom 3 VR
- Vanishing Realms
- Gunheart
- Fallout VR...?

Other games may not have touchpad movement options however this driver will 
also activate teleport.


# Features

- Change HMD Step Thresholds
- Change Vive Controller Swinging Thresholds
- Profiles for different games

# Upcoming

- Fixes for teleport games
- Options for emulating other input methods
- Tracker support (for feet)

# Notes:

This is a work-in-progress and may have flaws.

## Installer

Download the newest installer from the [release section](https://github.com/pottedmeat7/OpenVR-WalkInPlace/releases) and then execute it. Don't forget to exit SteamVR before installing/de-installing.

# Documentation

### WalkInPlace Overlay
Just enable "Step Detection" in the UI.

Enable the "analog" locomotion in the games settings this is the input method that uses touching the touch-pad (not clicking)
Then you simply walk in place to emulate walking in the VR world.

### Game Type
These options are for different combinations of controls some games use the touch-pad for movement and then a click to sprint, some use the click to engage teleport. If you dont want to trigger teleport use the second option.

The third option is for teleport only games, it is not working as desired at the moment.

### HMD Thresholds
The Y value is the Up and Down movement of your head to trigger a step, in order to trigger the real time HMD values have to be greater than the Y threshold.

The XZ value is the Side to Side movement to trigger a step, in order to trigger a step the real time HMD values have to be less than the XZ threshold.

### Hand Jog / Run
These values are for the magnitude of movement (X, Y and Z movement) of the arms. 

The real time Controller values have to be greater then these values in order to Jog / Run.

### Touch Options
These values will be applied to touch-pad locomotion for in order to apply different degrees of movement into the game.

### Profiles
If you like your current settings for a game and want to save them you can click "New Profile" it will take the current settings and save them with the profile name of your choice. 

After re-opening SteamVR you can reload your saved profiles by first clicking "Load Profiles" then selecting the profile you want from the drop down menu, and click "Apply".

If you want to update a profile with new settings you need to select the profile and delete it and re-create a "New Profile".

## Initial Setup
### Boost
1. Goto https://sourceforge.net/projects/boost/files/boost-binaries/1.63.0/
1. Download Boost 1.63 Binaries (boost_1_63_0-msvc-14.0-64.exe)
1. Install Boost into `OpenVR-WalkInPlace/third-party/boost_1_63_0`
  
### Qt
1. Goto https://download.qt.io/official_releases/qt/5.7/5.7.0/
1. Download Qt 5.7.0
1. Run the Qt installer (I installed it to "c:\Qt")
1. Goto `OpenVR-WalkInPlace\client_overlay`
1. Create `client_overlay.vcxproj.user` and paste the following into it:

```
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="14.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup>
    <QTDIR>C:\Qt\5.7\msvc2015_64</QTDIR>
  </PropertyGroup>
</Project>
```

NOTE: Adjust the path the `msvc2015_64` folder in Qt to match your installation

Then run the `windeployqt.bat` if your system doesn't find the exe update the batch to the absolute path
in `{QT_INSTLATION_PATH}\5.7\msvc2015_64\bin\windeployqt.exe`

## Building
1. Open *'VRWalkInPlace.sln'* in Visual Studio 2015.
2. Build Solution


# Known Bugs

- The shared-memory message queue is prone to deadlock the driver when the client crashes or is exited ungracefully.

# License

This software is released under GPL 3.0.
