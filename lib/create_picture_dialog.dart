import 'package:flutter/material.dart';
import 'package:gallery/api_service.dart';

class CreatePictureDialog extends StatefulWidget {
  final ApiService apiService;
  final String albumName;

  const CreatePictureDialog({
    Key? key,
    required this.apiService,
    required this.albumName,
  }) : super(key: key);

  @override
  _CreatePictureDialogState createState() => _CreatePictureDialogState();
}

class _CreatePictureDialogState extends State<CreatePictureDialog> {
  String _pictureName = '';
  String _picturePath = '';

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      title: const Text('Create Picture'),
      content: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          TextField(
            decoration: const InputDecoration(labelText: 'Picture Name'),
            onChanged: (value) {
              setState(() {
                _pictureName = value;
              });
            },
          ),
          TextField(
            decoration: const InputDecoration(labelText: 'Picture Path'),
            onChanged: (value) {
              setState(() {
                _picturePath = value;
              });
            },
          ),
        ],
      ),
      actions: [
        TextButton(
          onPressed: () {
            Navigator.of(context).pop(false);
          },
          child: const Text('Cancel'),
        ),
        TextButton(
          onPressed: () async {
            // Create the picture using API service
            final isSuccess = await widget.apiService.addPictureToAlbum(
              widget.albumName,
              _pictureName,
              _picturePath,
            );
            Navigator.of(context).pop(isSuccess);
          },
          child: const Text('Create'),
        ),
      ],
    );
  }
}
