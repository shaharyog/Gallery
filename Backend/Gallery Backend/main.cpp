// main.cpp

#include <cpprest/http_listener.h>
#include "DatabaseAccess.h"
#include "Colors.h"
#include "Constants.h"
#include "GalleryAPI.h"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

int main()
{
    // Create an HTTP listener and bind it to a URI
    GalleryAPI galleryApi(BASE_URI);

    try
    {
        // Start listening for requests
        galleryApi.start();

        // Keep the server running until terminated
        std::string input;
        std::getline(std::cin, input);
    }
    catch (const std::exception& e)
    {
        std::cerr << RED << "Error: " << e.what() << RESET << '\n';
    }

    return 0;
}
