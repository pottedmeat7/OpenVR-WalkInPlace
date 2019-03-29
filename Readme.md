![language](https://img.shields.io/badge/Language-C%2B%2B11-green.svg)  ![dependencies](https://img.shields.io/badge/Dependencies-Boost%201.65-green.svg)  ![license_gpl3](https://img.shields.io/badge/License-GPL%203.0-green.svg)

# OpenVR-WalkInPlace

An OpenVR driver that applies virtual movement using a pedometer

The OpenVR driver adds software defined controllers, tracks movement of the Physical HMD and Controllers, then applies input to the software controllers as well as mirror all physical input.

## A history

https://sites.google.com/view/openvr-walkinplace/home


# Current Games that Work Best with OpenVR-WalkInPlace

- Skyrim VR
- The Forest
- Rec Room
- Arizona Sunshine
- Onward
- VR-Chat
- DOOM VFR
- Any other games with Analog locomotion controls

Other games may not have touchpad movement options however this driver will 
also activate teleport if you'd like.

# Features

- Movement Data Model recording
- Tracker support
- Configuration for "Arm Swinging" Locomotion
- Save profiles
- Device movement Graph

# Upcoming

- HMD Relative Direction Override to Controller Relative Direction
- Any Relative Direction to Tracker Relative Direction
 
## Installer

Download the newest installer from the [release section](https://github.com/pottedmeat7/OpenVR-WalkInPlace/releases) and then execute it. Don't forget to exit SteamVR before installing/de-installing.

# Documentation

## Configuration Examples

![Example Screenshot](docs/screenshots/walkinplace_default.png)
*These are settings which work for many games

## GIF Step Examples
Walking Example: 
![walk example](docs/gifs/wip_walk.gif)

Jogging Example: 
![jog example](docs/gifs/wip_jog.gif)

Running Example: 
![run example](docs/gifs/wip_run.gif)

Direction Control Example: 
![directions example](docs/gifs/wip_dir.gif)

![Example Screenshot](docs/screenshots/wip_graph.png)
*This is what the graph should look like when walking in place


## WalkInPlace Overlay
Check "Enable WIP" to activate the virtual input
(also enable the "analog" locomotion in the games settings this is the input method that uses the touch-pad)

### HMD Type
Choose which HMD your using

### Input Type
There are a few different methods of inputs used by games
- The standard "Touchpad" locomotion games, that use from 0-1 on the touchpad for forward movement speed, often these games also have a "click to sprint" function. Use either "Touchpad (with click)" or "Touchpad" also "Joystick (with click)" or "Joysitck" for Rift
- Some games use touchpad locomotion except you have to press and hold the touchpad while moving your finger from 0-1 use the option "Touchpad (pressed)" or "Joystick (pressed)" for these games
- Anything else such as Teleport games just generating a click with "click to sprint" is the best option

### Controller selection
All input now goes into the OVRWIP Custom Controller you can also rebind the input from the OVRWIP controller to other controllers using the SteamVR Input Bindings.

### buttons to disable/enable WIP
These options can be used to disable/enable virtual movement when your holding or not holding the button selected.
- First choose either "enable WIP" or "disable WIP"  from the first drop down
- Then choose the button action that you'd prefer from the second drop down
- Then choose which controller that you want the button action to be on

## Enable / Disable Device page

### Use Trackers?
Enables/Disable trackers. This will use both the HMD thresholds and tracker thresholds in order to trigger a step.

### Track HMD Velocity? Track HMD Rotation?
This will disable the tracking of the HMD movement, and will only utilize the thresholds of the tracker movement. 

### Data Models
You can record a data model of your HMD, tracker and controller movement rates to use to match the similar movement in real-time.
You can Create, Apply, and Delete the data models. The current applied Data Model, will the one saved into a profile if you create one. You can also choose any pre-recorded data models without a profile as well.

## Data Model Recording
Follow the on screen popups instructions when recording a new data model. The recording process is from slowest to fastest movement of all devices. There will be 5 steps in the following order, Slowest Pace (5 seconds), Slow Scaling up to Medium (5 Second), Medium Paced (5 Seconds), Medium Scaling up to Fast (5 Seconds), and Fastest Pace. 

As you record the touch values are recorded in the following order Slow (0 touch value), Slow to medium (0-0.5 touch value), Medium (0.5 touch value), Medium to fast (0.5-1.0 touch value), and Fast (1.0 touch value)

Mostly it will be your controller values that are used to determine where in the model to pick the corresponding touch value from.
The HMD and trackers mainly determine if you are moving at all according to the model.

If you find that you move to quickly to soon, try re-recording a model with more significant arm swinging.

### Apply Data Model
Make sure to "apply" the data model before Enabling WIP or trying to view the data model as well.

## Show Data Model Page

### Show HMD, Show Trackers, Show Controller, Show Touch Line
These show the models of each device. The Touch Line is the values inserted from 0-1 of the progress during the recording. 
The touch-line will be scaled to be between the values in the "Touch Pace Options". Ie. From Min-Max.

### Touch Pace Options
These values control the degree of movement applied in game.
Some games will use the touchpad axis differntly, for slow games sometimes there is only 1 degree of movment.
Some games use the entire axis from the center, 0, to 1

If you find the walking with just the HMD is too sensitive you can set the "Walk Touch" to 0 this will require your HMD and arms to swing in order to trigger a step via triggering the "mid" or "max" touch value.

### Profiles
If you like your current settings for a game and want to save them you can click "New Profile" it will take the current settings and save them with the profile name of your choice. 
If you want to update a profile with new settings you need to select the profile and delete it and re-create a "New Profile".

If you name a profile with the name "default" it will be the initially loaded profile once you start SteamVR. If you dont want to automatically enable walk in place locomotion on startup save the default profile with "enableWIP" unchecked.

## Graph Page
The graph page will show you realtime values from the HMD, controllers and trackers.
If you have WIP enabled the graph page will display vertical lines yellow for walk detection, orange for jog detection and red for run detection.

## Any Issues Check out the Logs
Overlay UI Log here `C:\Users\<USERNAME>\AppData\Roaming\pottedmeat7\OpenVRWalkInPlace\VRWalkInPlace.log`
Driver Log here `C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers\00vrwalkinplace\bin\win64\driver_vrwalkinplace.log`


## Setting up To Re-Build the project from Source
*You do not need this if you just want to use the WIP overlay*

### Boost
- WINDOWS
	1. Goto https://sourceforge.net/projects/boost/files/boost-binaries/1.65.1/
	2. Download Boost 1.65 Binaries (boost_1_65_1-msvc-14.1-64.exe)
	3. Install Boost into `OpenVR-WalkInPlace/third-party/boost_1_65_1`
- LINUX
	1. Go to https://sourceforge.net/projects/boost/files/boost/1.65.1/
	2. Download boost_1_65_1.tar.gz
	3. Extract the files into `OpenVR-WalkInPlace/third-party/boost_1_65_1`
  
### Qt
1. Goto https://download.qt.io/official_releases/qt/5.9/5.9.0/
2. Download Qt 5.9.0 (Either windows .exe or linux .run file)
3. Run the Qt installer (I installed it to `c:\Qt` or `/home/<user>/` on linux)

### OpenBlas
- WINDOWS
	1. Open the NuGet packages manager (Tools > NuGet Package Manager > Manage NuGet Packages for Solution...)
	2. Click on the “Browse” tab and search for “openblas”
	3. Click on OpenBlas and check the mlpack project, then click Install
	4. Once it has finished installing, close Visual Studio

## Building
- WINDOWS
	Build *'VRWalkInPlace.sln'* in Visual Studio 2017
- LINUX	
	Build the OpenVR-WalkInPlaceOverlay.pro project with Qt Creator 
	from a shell run
	`./build.sh`
	go to https://github.com/probonopd/linuxdeployqt/releases and download the AppImage
	then run
	`cd <OpenVR-WalkInPlace path>/client_overlay/bin/x64/
	<linuxdeployqt path>/linuxdeployqt-5-x86_64.AppImage OpenVR-WalkInPlaceOverlay -qmldir=res/qml/ -no-translations -bundle-non-qt-libs -appimage -verbose=2`

### Building installer
1. go to https://sourceforge.net/projects/nsis/files/NSIS%202/2.33/
2. download and run the nsis-2.33-setup.exe
3. go to `OpenVR-WalkInPlace/installer`
4. right click the `installer.nsi` file and `Compile NSIS Script`
5. the installer exe will be built into the same directory

## Uninstall
1. Run "C:\Program Files\OpenVR-WalkInPlace\Uninstall.exe" will remove everything

# Known Bugs

- The shared-memory message queue is prone to deadlock the driver when the client crashes or is exited ungracefully.

# License

This software is released under GPL 3.0.
