# sc4-resource-loading-hooks

A DLL Plugin for SimCity 4 that allows other DLLs to modify resources as the game loads them.   

The plugin can be downloaded from the Releases tab: https://github.com/0xC0000054/sc4-resource-loading-hooks/releases

## Technical Details

The plugin replaces the exemplar loading class that the game's resource manager uses with a proxy/wrapper class that
forwards the request to the game's original exemplar loading class and lets a DLL modify the loaded exemplar before it
is returned to the game.

## Using the Code

1. Copy the headers from `src/public/include` folder into your GZCOM DLL project.
2. Implement `cIExemplarLoadHookTarget` and/or `cIExemplarLoadErrorHookTarget` as additional interface(s) on
your GZCOM DLL director.
3. Register for the exemplar load and/or error notifications in `PreAppInit`.

See [LogExemplarTGIDllDirector.cpp](src/public/source/LogExemplarTGIDllDirector.cpp) for an example implementation.    
[ExemplarLoadLoggingDllDirector.cpp](src/child-directors/exemplar-load-logging/ExemplarLoadLoggingDllDirector.cpp) provides an example of
using the `cIExemplarLoadErrorHookTarget` interface.  

## System Requirements

* SimCity 4 version 641
* Windows 10 or later

The plugin may work on Windows 7 or later with the [Microsoft Visual C++ 2022 x86 Redistribute](https://aka.ms/vs/17/release/vc_redist.x86.exe) installed, but I do not have the ability to test that.

## Installation

1. Close SimCity 4.
2. Copy `SC4ResourceLoadingHooks.dll` into the Plugins folder in the SimCity 4 installation directory.
3. Start SimCity 4.

## Command Line Argument

The plugin adds an `-exemplar-log:` command line argument that controls what data is logged, the argument supports
the following values:

* `error` - Only exemplar load errors are logged.
* `TGI` - Logs the exemplar TGI.
* `type` - Logs the exemplar TGI and type (if present).
* `debug` - Logs the exemplar TGI, type (if present), and the name of the method used to load the exemplar.
* `<exemplar type id>` - Logs the TGI of exemplars matching `<exemplar type id>`, e.g. `-exemplar-log:0x21`
would log the TGI values of all 'Type 21' exemplars.

The log will be written to a `SC4ExemplarLoad.log` file in the same folder as the plugin.
The logging will also slow down your game.

## Troubleshooting

The plugin should write a `SC4ResourceLoadingHooks.log` file in the same folder as the plugin.    
The log contains status information for the most recent run of the plugin.

# License

This project is licensed under the terms of the GNU Lesser General Public License version 3.0.    
See [LICENSE.txt](LICENSE.txt) for more information.

## Public GZCOM Interface Headers and Example Code

The sub-folders of src/public contain the public interface headers for the GZCOM classes that this DLL
provides and example code demonstrating their use.    
These files are separately licensed under the terms of the MIT License (https://opensource.org/license/mit).   

## 3rd party code

[gzcom-dll](https://github.com/nsgomez/gzcom-dll/tree/master) Located in the vendor folder, MIT License.    
[EABase](https://github.com/electronicarts/EABase) Located in the vendor folder, BSD 3-Clause License.    
[EASTL](https://github.com/electronicarts/EASTL) Located in the vendor folder, BSD 3-Clause License.    
[Windows Implementation Library](https://github.com/microsoft/wil) - MIT License    
[Boost.Algorithm](https://www.boost.org/doc/libs/1_84_0/libs/algorithm/doc/html/index.html) - Boost Software License, Version 1.0.    

# Source Code

## Prerequisites

* Visual Studio 2022

## Building the plugin

* Open the solution in the `src` folder
* Update the post build events to copy the build output to you SimCity 4 application plugins folder.
* Build the solution

## Debugging the plugin

Visual Studio can be configured to launch SimCity 4 on the Debugging page of the project properties.
I configured the debugger to launch the game in full screen with the following command line:    
`-intro:off -CPUcount:1 -w -CustomResolution:enabled -r1920x1080x32 -exemplar-log:type`

You may need to adjust the window resolution for your primary screen.
