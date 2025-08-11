# 🚀 http-server-cpp

A multithreaded C++ HTTP server with static file serving, custom routing, and basic security features. Inspired by [codecrafters](https://app.codecrafters.io/courses/http-server/overview).

## 🛠️ Build & Run

Requirements: CMake, GCC

### Windows
```powershell
.\build.ps1      # Build and test
.\run.ps1        # Build, test, and run server
```

### Unix/Linux/Mac
```bash
./build.sh       # Build and test
./run.sh         # Build, test, and run server
```

## 🌐 Features
- ⚡ Fast, multithreaded HTTP server
- 🗂️ Static file serving from `/www`
- 🔀 Custom routing with regex support
- 🛡️ Security against directory traversal
- 🧪 Unit tests with [Google Test](https://github.com/google/googletest)

## 📚 Key Source Files
- [`src/main.cpp`](./src/main.cpp) — Main entry point
- [`src/server/httpserver.hpp`](./src/server/httpserver.hpp) — Server class
- [`src/server/router.hpp`](./src/server/router.hpp) — Routing logic
- [`src/http/httprequest.hpp`](./src/http/httprequest.hpp) — HTTP request parsing
- [`src/http/httpresponse.hpp`](./src/http/httpresponse.hpp) — HTTP response formatting

## 📄 Example Static Files
- [`www/index.html`](./www/index.html) — Home page
- [`www/about.html`](./www/about.html) — About page
- [`www/styles.css`](./www/styles.css) — CSS styles
- [`www/script.js`](./www/script.js) — Demo JavaScript
- [`www/images/monkey.jpg`](./www/images/monkey.jpg) — Sample image

## 📁 Project Structure

```
http-server-cpp/
├── build.ps1           # Windows build script (PowerShell)
├── build.sh            # Unix build script (Bash)
├── run.ps1             # Windows run script (PowerShell)
├── run.sh              # Unix run script (Bash)
├── CMakeLists.txt      # CMake build configuration
├── src/                # Source code
│   ├── main.cpp        # Entry point
│   ├── helpers.hpp     # Utility functions
│   ├── http/           # HTTP protocol logic
│   │   ├── httpcode.hpp
│   │   ├── httpmethod.hpp
│   │   ├── httprequest.cpp/.hpp
│   │   ├── httpresponse.cpp/.hpp
│   ├── server/         # Server implementation
│   │   ├── httpserver.cpp/.hpp
│   │   ├── router.cpp/.hpp
│   │   ├── socket_wrapper.hpp
├── tests/              # Unit tests (Google Test)
│   ├── tests_httprequest.cpp
│   ├── tests_httpresponse.cpp
│   ├── tests_router.cpp
├── www/                # Static web files
│   ├── index.html
│   ├── about.html
│   ├── styles.css
│   ├── script.js
│   └── images/
│       └── monkey.jpg
└── .github/            # GitHub workflows & instructions
```

## 🧪 Testing
Tests are located in [`tests/`](./tests/):
- [`tests_httprequest.cpp`](./tests/tests_httprequest.cpp)
- [`tests_httpresponse.cpp`](./tests/tests_httpresponse.cpp)
- [`tests_router.cpp`](./tests/tests_router.cpp)

Run tests automatically with the build scripts.

## 🤝 Contributing
Pull requests and issues are welcome! See [Google Test](https://github.com/google/googletest) for testing framework info.

## 📄 License
This project is licensed under the MIT License.
