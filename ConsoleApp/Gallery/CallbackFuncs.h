#pragma once

// general callback functions
int existenceCallback(void* data, int argc, char** argv, char** azColName);
int countCallback(void* data, int argc, char** argv, char** azColName);
int getIDCallback(void* data, int argc, char** argv, char** azColName);


// album related callbacks
int getAlbumCallback(void* data, int argc, char** argv, char** azColName);
int getAlbumsCallback(void* data, int argc, char** argv, char** azColName);


// user related callbacks
int getUsersCallback(void* data, int argc, char** argv, char** azColName);
int getUserCallback(void* data, int argc, char** argv, char** azColName);


// picture related callbacks
int getPictureCallback(void* data, int argc, char** argv, char** azColName);
int getPicturesCallback(void* data, int argc, char** argv, char** azColName);


// tag related callbacks
int getTagsCallback(void* data, int argc, char** argv, char** azColName);