# Shahar Yogev - Gallery Backend API

This is a simple RESTful API for managing albums and users in a gallery application.

## Requirements

- Visual Studio 2022
- vcpkg with SQLite3 and C++ REST SDK installed

## Setup

1. Clone this repository to your local machine.
2. Open the solution file (`Gallery Backend.sln`) in Visual Studio 2022.
3. Make sure that vcpkg is installed and configured correctly.
4. Install SQLite3 and C++ REST SDK using vcpkg: `vcpkg install sqlite3 cpprestsdk`
5. Build the solution in Visual Studio.
6. If you can't build the solution it might because of the visual studio version, in that case, go to `Properties` > `C/C++` > `Preprocessor` > `Preprocessor Definitions`, and add the `_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING` to the definitions

## Usage

1. Run the application from Visual Studio.
2. The API will start listening for requests at the specified URI.
3. Use an HTTP client (e.g., Postman) to send requests to the API endpoints.

## API Endpoints
- See the static documentation html [here](./documentation.html)