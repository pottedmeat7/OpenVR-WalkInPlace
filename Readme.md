![language](https://img.shields.io/badge/Language-C%2B%2B11-green.svg)  ![dependencies](https://img.shields.io/badge/Dependencies-Boost%201.65-green.svg)  ![license_gpl3](https://img.shields.io/badge/License-GPL%203.0-green.svg)

# OpenVR-WalkInPlace

An OpenVR driver that applies virtual movement using a pedometer

The OpenVR driver adds software defined controllers, tracks movement of the Physical HMD and Controllers, then applies input to the software controllers as well as mirror all physical input.

## A history

https://sites.google.com/view/openvr-walkinplace/home


# Current Games that Work Best with OpenVR-WalkInPlace

- Skyrim VR
- Rec Room
- Arizona Sunshine
- Onward
- VR-Chat
- DOOM VFR
- Any other games with Keyboard or Touchpad locomotion controls

Other games may not have touchpad movement options however this driver will 
also activate teleport if you'd like.

# Features

- Change Step Thresholds for Walk / Jog / Run in Place to fit different games
- Tracker support (for feet)
- Configuration for "Arm Swinging" Locomotion
- Change speed of movement applied in game 
- Profiles for different games
- graph of velocity values for step configuration

# Upcoming

- Auto Configure
- Options for emulating other input methods
- Fixes for teleport games

## Installer

Download the newest installer from the [release section](https://github.com/pottedmeat7/OpenVR-WalkInPlace/releases) and then execute it. Don't forget to exit SteamVR before installing/de-installing.

If you also use OpenVR-InputEmulator you can use the 'OpenVR-WalkInPlace-OVRIE-driver.exe' installer.

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

## Watch the Selected Controller touchpad to See Successful Virtual Input
When a step is detected touchpad input will be applied to the selected controller. 
In the SteamVR Overlay this input will be visualized with a small grey dot on the virtual touchpad of the selected controller. 

### Auto Conf
You can detect ideal HMD Thresholds, Tracker Configuration, and Jog/Run Arm swing values with Auto Conf function on the Graph Page

### WalkInPlace Overlay
Check "Enable WIP" to activate the virtual input
(also enable the "analog" locomotion in the games settings this is the input method that uses the touch-pad)

### HMD Type
Choose which HMD your using

### Input Type
There are a few different methods of inputs used by games
-The standard "Touchpad" locomotion games, that use from 0-1 on the touchpad for forward movement speed, often these games also have a "click to sprint" function. Use either "Touchpad (with click)" or "Touchpad" also "Joystick (with click)" or "Joysitck" for Rift
-Some games use the touchpad locomotion except you have to also press and hold the touchpad while moving your finger from 0-1 use the option "Touchpad (pressed)" or "Joystick (pressed)" for these games
-Some games only allow for teleport, or they have other teleport functions like Dash, for these you should use either "touchpad (with click)" or "joystick (with click)" you then should lower the "Arm Swing Run" value to 0.05 or very low also lower the "step time" very low to 0.05 this will trigger a click input and very quickly unclick. Its not perfect but I found for some games it is an improvement over teleporting over and over.
-Keyboard input is fallback that is only needed if nothing else works

### Controller selection (which controller is used for virtual input)
This menu allows you to select which controller should be used for virtual input.
The selected controller will highlight green for 10 seconds

### buttons to disable/enable WIP
These options can be used to disable/enable virtual movement when your holding or not holding the button selected.
- First choose either "enable WIP" or "disable WIP"  from the first drop down
- Then choose the button action that you'd prefer from the second drop down
- Then choose which controller that you want the button action to be on

### HMD Thresholds
The Y value is the Up and Down movement of your head to trigger a step, in order to trigger the real time HMD values have to be greater than the Y threshold.

The XZ value is the Side to Side movement that will disable triggering a step (if over the threshold), in order to trigger a step the HMD values have to be less than the XZ threshold.

The HMD Y values have to be greater then the HMD XZ values a in order to take a step as well as both Y and XZ meet the threshold.

### Tracker Thresholds
These values are functionality the same as the HMD values except from a tracker device. 

### Use Trackers?
Enables/Disable trackers. This will use both the HMD thresholds and tracker thresholds in order to trigger a step.

### Disable HMD?
This will disable the tracking of the HMD movement, and will only utilize the thresholds of the tracker movement. 
You must have both "Use Trackers?" Checked as well as "Disable HMD?" checked. 

### Arm Swing Jog / Arm Swing Run
These values are for the controller Up and Down movement of the arms. 
The real time Controller values have to be greater then these values in order to Jog / Run.

### Step Time
This is how long each "step" lasts. If 1 step is detected it will last this amount of time in seconds. As you repeat steps this time is reset for every step detected. 

### Touch Options
These values control the degree of movement applied in game.
Some games will use the touchpad axis differntly, for slow games sometimes there is only 1 degree of movment.
Some games use the entire axis from the center, 0, to 1

If you find the walking with just the HMD is too sensitive you can set the "Walk Touch" to 0 this will require your HMD and arms to swing in order to trigger a step via triggering the "jog" or "run" touch value with the "arm swing" thresholds above.

### Scale Touch with Swing?
This will enable varying touch values between your minimum walk/jog/run values depending on how much your average arm swinging values (average for "step time" times 3) differ from the "jog/run" swing values. The default "walk" arm swing value is assumed to be zero so anything over that will scale over the "walk touch" value.

### Use Controller Direction for Straf? and Use Controller Direction for Reverse?
These are made for games that either have HMD relative movement (ie you move the direction you face). These games do allow straf movement often by touching the left or right side of the touchpad. These options will calcuate your controller direction and use left or right touchpad values if your controllers direction is perpendicular to your HMD forward direction. 
Some games also do not support controller direction for reverse, this option will determine if your controller is pointed backwards and apply negative values for the touchpad. 
(These features currently have gimbal lock problems, and are having more conflicts when both options are used)

### Profiles
If you like your current settings for a game and want to save them you can click "New Profile" it will take the current settings and save them with the profile name of your choice. 
After re-opening SteamVR you can reload your saved profiles by first clicking "Load Profiles" then selecting the profile you want from the drop down menu, and click "Apply".
If you want to update a profile with new settings you need to select the profile and delete it and re-create a "New Profile".
If you name a profile with the name "default" it will be the initially loaded profile once you "load profiles".

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
