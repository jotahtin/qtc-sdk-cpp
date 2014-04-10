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

TODO

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
