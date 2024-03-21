import 'dart:io';

import 'package:flutter/material.dart';
import 'package:gallery/api_service.dart';
import 'package:gallery/picture.dart';
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
  late Future<List<Picture>> _fetchPicturesFuture;

  @override
  void initState() {
    super.initState();
    _fetchPicturesFuture = fetchPictures();
  }

  Future<List<Picture>> fetchPictures() async {
    return widget.apiService
        .getAlbumPictures(widget.albumOwnerID, widget.albumName);
  }

  Future<void> _showCreatePictureDialog() async {
    final result = await showDialog<bool>(
      context: context,
      builder: (context) => CreatePictureDialog(
        apiService: widget.apiService,
        albumName: widget.albumName,
      ),
    );
    if (result == true) {
      // Refresh pictures after creating a new picture
      setState(() {
        _fetchPicturesFuture = fetchPictures();
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('${widget.albumName} - ${widget.albumOwnerName}'),
      ),
      body: FutureBuilder<List<Picture>>(
        future: _fetchPicturesFuture,
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            // Display a skeleton loader while fetching pictures
            return const Center(child: CircularProgressIndicator());
          } else if (snapshot.hasError) {
            // Handle error state
            return Center(
              child: Text('Error: ${snapshot.error}'),
            );
          } else if (snapshot.hasData) {
            // Display fetched pictures
            final pictures = snapshot.data!;
            return GridView.builder(
              gridDelegate: const SliverGridDelegateWithFixedCrossAxisCount(
                crossAxisCount: 3, // Adjust the number of columns as needed
                mainAxisSpacing: 8,
                crossAxisSpacing: 8,
              ),
              itemCount: pictures.length,
              itemBuilder: (context, index) {
                return _buildPictureTile(pictures[index]);
              },
            );
          } else {
            // Handle empty state
            return const Center(
              child: Text('No pictures found'),
            );
          }
        },
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _showCreatePictureDialog,
        child: const Icon(Icons.add),
      ),
    );
  }

  Widget _buildPictureTile(Picture picture) {
    return GestureDetector(
      onTap: () {
        // Handle onTap event if needed
      },
      child: Stack(
        children: [
          Image.file(
            File(picture.path),
            width: double.infinity,
            height: double.infinity,
            fit: BoxFit.cover,
            errorBuilder: (context, error, stackTrace) {
              return const Text('Invalid Image');
            },
          ),
          Positioned(
            bottom: 0,
            left: 0,
            right: 0,
            child: Container(
              color: Colors.black.withOpacity(0.5),
              padding: const EdgeInsets.all(4),
              child: Text(
                picture.name,
                style: const TextStyle(color: Colors.white),
                textAlign: TextAlign.center,
                maxLines: 1,
                overflow: TextOverflow.ellipsis,
              ),
            ),
          ),
        ],
      ),
    );
  }
}
