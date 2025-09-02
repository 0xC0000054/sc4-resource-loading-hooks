# sc4-resource-loading-hooks

A DLL Plugin for SimCity 4 that allows DLLs to modify resources as the game loads them.   

The plugin can be downloaded from the Releases tab: https://github.com/0xC0000054/sc4-resource-loading-hooks/releases

## Exemplar Patching

Exemplar patching is a technique to change the behavior of Exemplar files on-the-fly at runtime, without altering the original DBPF files.
An **Exemplar Patch** is a Cohort file with Group ID 0xb03697d1 (and arbitrary Instance ID) containing the property:

- `Exemplar Patch Targets` (property id 0x0062e78a): list of Exemplar files this patch applies to
  (format: Group ID 1, Instance ID 1, Group ID 2, Instance ID 2,…).
  The list must contain an even number of IDs.

All the other properties of the Cohort file (except for `Exemplar Name`) are injected into these target Exemplar files
whenever the game loads an Exemplar file from this list.
This allows to add or overwrite specific properties of Exemplar files without affecting any unrelated properties.

You can think of an Exemplar Patch as a "Child Cohort" file, mirroring the usual Parent Cohort structure.
An Exemplar Patch can target multiple Exemplar files simultaneously.
Conversely, multiple Exemplar Patches can be applied to the same Exemplar file.


                Parent Cohort
                 /        \
                /          \
               /            \
              /              \
       Exemplar A          Exemplar B
           |   \_____       /   |
           |    _____\_____/    |
           |   /      \______   |
           |  /              \  |
    Exemplar Patch 1     Exemplar Patch 2


If two Cohort files have the same Instance ID, only the one loading last has an effect.
Therefore, you should usually change the Instance ID whenever you copy an existing Exemplar Patch Cohort file.

## For DLL Developers

1. Copy the headers from `src/public/include` folder into your GZCOM DLL project.
2. Implement `cIExemplarLoadHookTarget` and/or `cIExemplarLoadErrorHookTarget` as additional interface(s) on
your GZCOM DLL director.
3. Register for the exemplar load and/or error notifications in `PreAppInit`.

See [LogExemplarTGIDllDirector.cpp](src/public/examples/LogExemplarTGIDllDirector.cpp) for an example implementation.    

## System Requirements

* SimCity 4 version 641
* Windows 10 or later

The plugin may work on Windows 7 or later with the [Microsoft Visual C++ 2022 x86 Redistribute](https://aka.ms/vs/17/release/vc_redist.x86.exe) installed, but I do not have the ability to test that.

## Installation

1. Close SimCity 4.
2. Copy `SC4ResourceLoadingHooks.dll` into the Plugins folder in the SimCity 4 installation directory.
3. Start SimCity 4.

## Command Line Arguments

### Exemplar Load Logging

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

### Exemplar Patch Debug Logging

The plugin adds an `-exemplar-patch-debug-logging` command line argument that enables more detailed
logging of the exemplar patching process.    
By default, the plugin will only log the number of exemplar patches and total number of exemplars that
they target, e.g. 'Loaded 10 Exemplar patches targeting, in total, 41 Exemplar files.'.

The log will be written to a `SC4ResourceLoadingHooks.log` file in the same folder as the plugin.

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

## 3rd Party Code and Credits

[gzcom-dll](https://github.com/nsgomez/gzcom-dll/tree/master) - MIT License.   
[Windows Implementation Library](https://github.com/microsoft/wil) - MIT License    
[Boost.Algorithm](https://www.boost.org/library/latest/algorithm/) - Boost Software License, Version 1.0.    
[Boost.Container](https://www.boost.org/library/latest/container/) - Boost Software License, Version 1.0.    
[Boost.Unordered](https://www.boost.org/library/latest/unordered/) - Boost Software License, Version 1.0.    
[submenus-dll](https://github.com/memo33/submenus-dll) - GNU Lesser General Public License version 3.0

The Exemplar Patching code and documentation is based on the implementation in submenus-dll.

# Source Code

## Prerequisites

* Visual Studio 2022
* `git submodule update --init`
* [VCPkg](https://github.com/microsoft/vcpkg) with the Visual Studio integration

## Building the plugin

* Open the solution in the `src` folder
* Update the post build events to copy the build output to you SimCity 4 application plugins folder.
* Build the solution

## Debugging the plugin

Visual Studio can be configured to launch SimCity 4 on the Debugging page of the project properties.
I configured the debugger to launch the game in full screen with the following command line:    
`-intro:off -CPUcount:1 -w -CustomResolution:enabled -r1920x1080x32 -exemplar-log:type`

You may need to adjust the window resolution for your primary screen.
