# Easel
This is a rendering framework built to help in the initial setup of simulations.

## Details
This project is built using MSVC with the Visual Studio 2019 toolset (v142) for the C++17 standard.

## Dependencies
Part of the point of making this project was to attempt to create a basic rendering system while using as few libraries as possible. While I'm still committed to this goal, certain features I would like to implement are too impractical to try to learn and create myself while still focusing my own growth in graphics programming specifically, such as Audio or Online Play. The following is a list of the external libraries I'll be making use of and for what purpose.
* [DirectX Toolkit 2017](https://github.com/microsoft/DirectXTK) (NuGet)
  * Reading image files for texture generation
* [Assimp 3.0.0](http://www.assimp.org/) (NuGet)
  * Loading 3D Models
  
## Screenshots
![Normal Mapping Demo](https://github.com/rubenaryo/Easel/blob/master/images/screen1.PNG?raw=true)

## Visual Studio Configuration
The development environment used is Visual Studio 2019, configured such that
* On compilation, all binary executables get placed in the "_Binary" folder
  * Debug builds have a "_d" suffix
* All intermediate files (.suo, .obj, .pdb, etc) are placed in the "Z_DELETE" folder
  * This folder is always safe to delete
  * Running 'Clean' deletes the contents of this folder, but keeps the log files
* The "System" folder contains all .cpp, .h, .hpp files for booting up the app, 
which the IDE internally sorts into Headers and Source Files in the Solution Explorer

These configuration settings were set by using the Solution Properties GUI (Alt + Enter) and stored in the included '.vcxproj' and '.vcxproj.filters' files, so feel free to change them as you like!

This config was heavily inspired by one used by a professor of mine, which I thought was good enough to use for all my projects.

## Performance Analysis
A performance analysis for this engine is ongoing, the first section has been uploaded as Easel_PerformanceAnalysis.pdf. Section 2 will be uploaded on October 28, 2020.

## Character Set
This was built to support the Unicode character set, meaning it is based on using wstrings rather than strings.
