import 'dart:io';

import 'package:flutter/material.dart';
import 'package:gallery/api_service.dart';
import 'package:gallery/error_dialog.dart';
import 'package:gallery/picture.dart';
import 'package:gallery/tags_dialog.dart';
import 'create_picture_dialog.dart';

class PicturesPage extends StatefulWidget {
  final int albumOwnerID;
  final String albumOwnerName;
  final String albumName;
  final ApiService apiService;
  final int picturesCount;

  const PicturesPage({
    super.key,
    required this.albumOwnerID,
    required this.albumOwnerName,
    required this.albumName,
    required this.apiService,
    required this.picturesCount,
  });

  @override
  _PicturesPageState createState() => _PicturesPageState();
}

class _PicturesPageState extends State<PicturesPage> {
  late List<Picture> _pictures = [];

  @override
  void initState() {
    super.initState();
    _refresh();
  }

  Future<void> _fetchPictures() async {
    try {
      final pictures = await widget.apiService
          .getAlbumPictures(widget.albumOwnerID, widget.albumName);
      setState(() {
        _pictures = pictures;
      });
    } catch (e) {
      _showErrorDialog('Error fetching pictures: $e');
    }
  }

  Future<void> _showCreatePictureDialog() async {
    await showDialog<bool>(
      context: context,
      builder: (context) => CreatePictureDialog(
        apiService: widget.apiService,
        albumName: widget.albumName,
      ),
    );

    _refresh();
  }

  void _refresh() {
    _fetchPictures();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('${widget.albumName} - ${widget.albumOwnerName}'),
        actions: [
          IconButton(
            onPressed: _refresh,
            icon: const Icon(Icons.refresh),
          ),
        ],
      ),
      body: _pictures.isEmpty
          ? const Center(
              child: Text('No pictures found'),
            )
          : GridView.builder(
              gridDelegate: const SliverGridDelegateWithFixedCrossAxisCount(
                crossAxisCount: 3, // Adjust the number of columns as needed
                mainAxisSpacing: 8,
                crossAxisSpacing: 8,
              ),
              itemCount: _pictures.length,
              itemBuilder: (context, index) {
                return _buildPictureTile(_pictures[index]);
              },
            ),
      floatingActionButton: FloatingActionButton(
        onPressed: _showCreatePictureDialog,
        child: const Icon(Icons.add),
      ),
    );
  }

  Widget _buildPictureTile(Picture picture) {
    return Dismissible(
      key: Key(picture.id.toString()),
      direction: DismissDirection.endToStart,
      background: Container(
        color: Colors.red,
        alignment: Alignment.centerRight,
        padding: const EdgeInsets.only(right: 20.0),
        child: const Icon(Icons.delete, color: Colors.white),
      ),
      onDismissed: (direction) async {
        await _deletePicture(picture);
      },
      child: GestureDetector(
        onTap: () async {
          await showDialog(
            context: context,
            builder: (context) => TagsDialog(
              apiService: widget.apiService,
              albumName: widget.albumName,
              pictureName: picture.name,
              pictureID: picture.id,
            ), // Show the tags dialog
          );
          _refresh();
        },
        child: Container(
          alignment: Alignment.bottomCenter,
          child: Stack(
            alignment: Alignment.bottomCenter,
            children: [
              Center(
                child: Image.file(
                  File(picture.path),
                  width: double.infinity,
                  height: double.infinity,
                  fit: BoxFit.cover,
                  errorBuilder: (context, error, stackTrace) {
                    return const Text('Invalid Image');
                  },
                ),
              ),
              Container(
                color: Colors.black.withOpacity(0.5),
                padding: const EdgeInsets.all(4),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  crossAxisAlignment: CrossAxisAlignment.center,
                  children: [
                    Text(
                      picture.name,
                      style: const TextStyle(color: Colors.white),
                      textAlign: TextAlign.center,
                      maxLines: 1,
                      overflow: TextOverflow.ellipsis,
                    ),
                    const Text(' • '),
                    Text(picture.formattedCreationDate,
                        style: const TextStyle(color: Colors.white),
                        textAlign: TextAlign.center,
                        maxLines: 1,
                        overflow: TextOverflow.ellipsis),
                    const Text(' • '),
                    Text('${picture.tagCount} ',
                        style: const TextStyle(color: Colors.white),
                        textAlign: TextAlign.center,
                        maxLines: 1,
                        overflow: TextOverflow.ellipsis),
                    const Icon(Icons.people, color: Colors.white),
                  ],
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }

  Future<void> _deletePicture(Picture picture) async {
    try {
      // Call the API service to delete the picture
      await widget.apiService
          .removePictureFromAlbum(widget.albumName, picture.name);
      setState(() {
        _pictures.remove(picture);
      });
    } catch (e) {
      _showErrorDialog(e.toString());
    }
  }

  void _showErrorDialog(String message) {
    showDialog(
      context: context,
      builder: (context) => ErrorDialog(
        message: message,
      ),
    );
  }
}
