import 'package:flutter/material.dart';
import 'package:gallery/api_service.dart';
import 'package:gallery/error_dialog.dart';
import 'package:gallery/user.dart';

class TagsDialog extends StatefulWidget {
  final ApiService apiService;
  final String albumName;
  final String pictureName;
  final int pictureID;

  const TagsDialog({
    super.key,
    required this.apiService,
    required this.albumName,
    required this.pictureName,
    required this.pictureID,
  });

  @override
  _TagsDialogState createState() => _TagsDialogState();
}

class _TagsDialogState extends State<TagsDialog> {
  List<User> taggedUsers = [];
  List<User> availableUsers = [];
  User? selectedUser;

  @override
  void initState() {
    super.initState();
    refresh();
  }

  Future<void> refresh() async {
    try {
      final taggedUsers = await widget.apiService
          .getPictureTags(widget.pictureID, widget.pictureName);
      final allUsers = await widget.apiService.getUsers();

      setState(() {
        availableUsers = allUsers;
        this.taggedUsers = taggedUsers;
      });
    } catch (e) {
      _showErrorDialog(e.toString());
    }
  }

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      title: Text("'${widget.pictureName}' Tags"),
      content: SingleChildScrollView(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: <Widget>[
            const Text("Tagged Users:"),
            for (var user in taggedUsers)
              ListTile(
                leading: const Icon(Icons.person),
                title: Text(user.name),
                subtitle: Text("ID: ${user.id}"),
                trailing: IconButton(
                  icon: const Icon(Icons.delete),
                  onPressed: () {
                    untagUser(user.id);
                  },
                ),
              ),
            const SizedBox(height: 20),
            const Text("Add Tag:"),
            DropdownButtonFormField<int>(
              value: selectedUser?.id,
              hint: const Text("Select User"),
              items: availableUsers.map((user) {
                return DropdownMenuItem<int>(
                  value: user.id,
                  child: Text(user.name),
                );
              }).toList(),
              onChanged: (selectedUserId) {
                setState(() {
                  selectedUser = availableUsers
                      .firstWhere((user) => user.id == selectedUserId);
                });
              },
            ),
            const SizedBox(height: 10),
            ElevatedButton(
              onPressed: () {
                if (selectedUser != null) {
                  tagUser(selectedUser!.id);
                } else {
                  _showErrorDialog("Please select a user.");
                }
              },
              child: const Text("Tag User"),
            ),
          ],
        ),
      ),
      actions: <Widget>[
        TextButton(
          onPressed: () {
            Navigator.of(context).pop(); // Close the dialog
          },
          child: const Text('Close'),
        ),
      ],
    );
  }

  void tagUser(int userId) async {
    try {
      await widget.apiService
          .tagUserInPicture(widget.albumName, widget.pictureName, userId);
      refresh();
    } catch (e) {
      _showErrorDialog(e.toString());
    }
  }

  void untagUser(int userId) async {
    try {
      await widget.apiService
          .untagUserInPicture(widget.albumName, widget.pictureName, userId);
      refresh();
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
