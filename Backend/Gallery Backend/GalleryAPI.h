#pragma once
#include <cpprest/http_listener.h>
#include "DatabaseAccess.h"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

class GalleryAPI {
public:
    GalleryAPI(const std::string& uri);
    const uri& listener_uri() const;

    void start();
    void handle_request(const http_request& request) const;

private:
    http_listener listener_;
    DatabaseAccess db_;

    // db related functions
    void clear_db(const http_request& request) const;

    // creation endpoints
    void create_album(const http_request& request) const;
    void create_user(const http_request& request) const;
    void add_picture_to_album(const http_request& request) const;
    void tag_user_in_picture(const http_request& request) const;

    // deletion endpoints
    void delete_user(const http_request& request) const;
    void delete_album(const http_request& request) const;
    void remove_picture_from_album(const http_request& request) const;
    void untag_user_in_picture(const http_request& request) const;

    // retrieval endpoints
    void get_albums(const http_request& request) const;
    void get_albums_of_user(const http_request& request) const;
    void get_users(const http_request& request) const;
    auto get_user(const http_request& request) const -> void;
    void get_user_albums_count(const http_request& request) const;
    void get_albums_tagged_user_count(const http_request& request) const;
    void get_count_tags_of_user(const http_request& request) const;
    void get_average_tags_of_user_per_album(const http_request& request) const;
    void get_album_pictures(const http_request& request) const;
    void get_picture_tags(const http_request& request) const;
};
