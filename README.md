# **dilog** - A tiny, modern, and safe logging library
<div align="center">

  <a title="'Build' workflow Status" href="https://github.com/diegomrno/dilog/actions/workflows/build.yml"><img alt="Build" src="https://img.shields.io/github/actions/workflow/status/diegomrno/dilog/build.yml?branch=main&label=Build&style=for-the-badge&logo=GitHub%20Actions&logoColor=fff"></a>
  <a title="Latest Release" href="https://github.com/diegomrno/dilog/releases/latest"><img alt="Latest Release" src="https://img.shields.io/github/v/release/diegomrno/dilog?style=for-the-badge&label=Release&logo=github"></a>
  <a title="Changelog" href="https://github.com/diegomrno/dilog/blob/main/CHANGELOG.md"><img alt="Changelog" src="https://img.shields.io/badge/Changelog-View-blue?style=for-the-badge&logo=readme&logoColor=white"></a>
</div>

> Work in progress...

**dilog** is a tiny, minimalistic logging utility designed for simplicity, security, and modern C standards. It's lightweight, memory-safe, thread-safe, and highly configurable for all your logging needs. Whether you're building a small utility or a large application, `dilog` is here to make logging easy and efficient.

> **Why this project**: Because I personally needed a really simple logging system tailored to my needs and I didn’t want to embed a full-blown *nuclear submarine* into my projects just to print "Hello, world!".

## **Usage**

To use `dilog`, simply include the `dilog.h` header file in your project. No external dependencies are required.

### Hello world with dilog 
```c
#include "../dilog.h"

int main() {
    dilog_init();
    dilog("Hello, world!");
    return 0;
}
```
(more doc incoming)

## **Features**

- 🗂️ **Topic System**: Log with flexible "profiles" (topics) that allow full customization — format, output options, and more.
- 🏷️ **Typed Logs**: Easily log messages as `error`, `info`, `warn`, or any custom-defined type.
- 💾 **[WIP] Log to a File**: Send your logs to a file for persistent tracking.
- 🌐 **Cross-Platform**: Works seamlessly on Linux, macOS, and Windows, no dependencies needed.
- 🎨 **Hex Color Support**: Customize your logs with full hex color codes for better readability and style.


