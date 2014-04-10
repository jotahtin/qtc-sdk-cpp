qtc-sdk-cpp
===========

# Qt Cloud Services SDK for C++

This is Qt Cloud Services SDK for C++. What is Qt Cloud Services? See below:

* The Qt Cloud Services home page is at https://www.qtc.io
* The Developer Documentation page is at https://developer.qtc.io

## Installing

### Using precomiled distributions

### Compiling from source code.

#### Linux

Preferred method to compile this library is using cmake.

````
cd <SourceDir>
mkdir -p build
cd build
cmake ..
make
````

#### Windows

##### 3rd Party libraries

Setting up 3rd party libraries in windows environment is inconvenient, because every library uses their own conventions. 
For this reason a prefer to install all 3rd party libraries in fixed directory structure with location is given by system wide environment variable 3RDPARTY_ROOT.

For example if one sets environment variable 3RDPARTY_ROOT=C:\3rdParty. Then I would expect following directory structure (not limited to).

C:\3rdParty\
  VC10_Win32
  VC10_x64
  VC11_Win32
    include
    lib
  VC11_x64
    include
    lib

So I simply install different 3rd party libraries under VC<VC version>_<Arch> using default unix style installation.

I regularly build some 3rd party libraries these are available in 


##### Visual Studio 2012 build using QtC-VC11.sln solution.



## Getting Started

You can find a getting started guide for Qt Cloud Services at:

http://developer.qtc.io/qtc/getting-started?snippets=cpp

If you are looking for service specific guides, please see:

* [Enginio Data Storage](http://developer.qtc.io/eds/getting-started?snippets=cpp)
* [Managed WebSocket](http://developer.qtc.io/mws/getting-started?snippets=cpp)

## Quick Start

This SDK is designed to be the most easy way possible to use Qt Cloud Services from C++. It supports most of the latest APIs. The following example shows how to use Enginio Data Storage (EDS) with the C++ SDK.

```
TODO
```

## Known issues

This SDK is currently under active development and there are still known issus with improved I/O stream handling. For this reason websockets and file transfer features are incomplete.


## SDK API References

See the product specific SDK API References:

* [Enginio Data Storage](https://github.com/qtcs/qtc-sdk-cpp/wiki/Enginio-Data-Storage-SDK-API)
* [Managed WebSocket](https://github.com/qtcs/qtc-sdk-cpp/wiki/Managed-WebSocket-SDK-API)
