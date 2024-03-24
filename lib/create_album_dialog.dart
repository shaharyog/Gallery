import 'package:flutter/material.dart';
import 'package:gallery/api_service.dart';
import 'package:gallery/user.dart';
import 'error_dialog.dart';

class CreateAlbumDialog extends StatefulWidget {
  final ApiService apiService;
  final List<User> users;

  const CreateAlbumDialog(
      {super.key, required this.apiService, required this.users});

  @override
  _CreateAlbumDialogState createState() => _CreateAlbumDialogState();
}

class _CreateAlbumDialogState extends State<CreateAlbumDialog> {
  late TextEditingController _albumNameController;
  late int _selectedUserId;

  @override
  void initState() {
    super.initState();
    _albumNameController = TextEditingController();
    _selectedUserId = widget.users.isNotEmpty ? widget.users[0].id : 0;
  }

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      title: const Text('Create Album'),
      content: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          TextField(
            controller: _albumNameController,
            decoration: const InputDecoration(labelText: 'Album Name'),
          ),
          DropdownButton<int>(
            value: _selectedUserId,
            onChanged: (value) {
              setState(() {
                _selectedUserId = value!;
              });
            },
            items: widget.users.map<DropdownMenuItem<int>>((user) {
              return DropdownMenuItem<int>(
                value: user.id,
                child: Text(user.name),
              );
            }).toList(),
          ),
        ],
      ),
      actions: [
        TextButton(
          onPressed: () {
            Navigator.pop(context);
          },
          child: const Text('Cancel'),
        ),
        TextButton(
          onPressed: () async {
            final albumName = _albumNameController.text;
            if (albumName.isNotEmpty) {
              try {
                await widget.apiService.createAlbum(albumName, _selectedUserId);
                Navigator.pop(context, true); // Pass true to indicate success
              } catch (e) {
                showDialog(
                  context: context,
                  builder: (context) => ErrorDialog(
                    message: e.toString(),
                  ),
                );
              }
            } else {
              _showErrorDialog(
                'Album name cannot be empty.',
              );
            }
          },
          child: const Text('Create'),
        ),
      ],
    );
  }

  void _showErrorDialog(String message) {
    showDialog(
      context: context,
      builder: (context) => ErrorDialog(
        message: message,
      ),
    );
  }

  @override
  void dispose() {
    _albumNameController.dispose();
    super.dispose();
  }
}
