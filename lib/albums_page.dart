import 'package:flutter/material.dart';
import 'package:gallery/api_service.dart';
import 'package:gallery/album.dart';
import 'package:gallery/pictures_page.dart';
import 'package:gallery/user.dart';

import 'create_album_dialog.dart';

class AlbumsPage extends StatefulWidget {
  final ApiService apiService;

  const AlbumsPage({Key? key, required this.apiService});

  @override
  _AlbumsPageState createState() => _AlbumsPageState();
}

class _AlbumsPageState extends State<AlbumsPage> {
  List<Album> albums = [];
  List<User> users = [];
  int? selectedUserId;

  @override
  void initState() {
    super.initState();
    fetchAlbums();
    fetchUsers();
  }

  Future<void> fetchAlbums() async {
    final fetchedAlbums = await widget.apiService.getAlbums();
    setState(() {
      albums = fetchedAlbums;
    });
  }

  Future<void> fetchUsers() async {
    final fetchedUsers = await widget.apiService.getUsers();
    setState(() {
      users = fetchedUsers;
    });
  }

  Future<void> fetchAlbumsOfUser(int userId) async {
    final fetchedAlbums = await widget.apiService.getAlbumsOfUser(userId);
    setState(() {
      albums = fetchedAlbums;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Albums'),
        actions: [
          IconButton(
            onPressed: () {
              fetchUsers();
              fetchAlbums();
            },
            icon: const Icon(Icons.refresh),
          ),
        ],
      ),
      body: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Padding(
            padding: const EdgeInsets.all(16.0),
            child: DropdownMenu<int>(
              dropdownMenuEntries: [
                const DropdownMenuItem<int>(
                  value: null,
                  child: Text('All Users'),
                ),
                ...users.map((user) {
                  return DropdownMenuItem<int>(
                    value: user.id,
                    child: Text(user.name),
                  );
                }),
              ],
              initialSelection: selectedUserId,
              label: const Text('Filter by user'),
              onSelected: (value) {
                setState(() {
                  selectedUserId = value;
                });
                if (value != null) {
                  fetchAlbumsOfUser(selectedUserId!);
                } else {
                  fetchAlbums();
                }
              },
            ),
          ),
          const SizedBox(height: 16),
          Expanded(
            child: ListView.builder(
              itemCount: albums.length,
              itemBuilder: (context, index) {
                return Dismissible(
                  key: Key(albums[index].name),
                  direction: DismissDirection.endToStart,
                  background: Container(
                    color: Colors.red,
                    alignment: Alignment.centerRight,
                    padding: const EdgeInsets.only(right: 20.0),
                    child: const Icon(Icons.delete, color: Colors.white),
                  ),
                  onDismissed: (direction) async {
                    await widget.apiService
                        .deleteAlbum(albums[index].name, albums[index].ownerId);
                    setState(() {
                      albums.removeAt(index);
                    });
                  },
                  child: ListTile(
                    title: Text(albums[index].name),
                    onTap: () {
                      Navigator.push(
                        context,
                        MaterialPageRoute(
                          builder: (context) => PicturesPage(
                            albumOwnerID: albums[index].ownerId,
                            albumName: albums[index].name,
                            apiService: widget.apiService,
                            picturesCount: albums[index].picturesCount,
                            albumOwnerName: albums[index].ownerName,
                          ),
                        ),
                      );
                    },
                    trailing: Row(mainAxisSize: MainAxisSize.min, children: [
                      Text("${albums[index].picturesCount} "),
                      const Icon(Icons.image)
                    ]),
                    subtitle: Row(mainAxisSize: MainAxisSize.min, children: [
                      Text(albums[index].ownerName),
                      // dot separator
                      const Text(' • '),
                      Text(albums[index].formattedCreationDate),
                    ]),
                  ),
                );
              },
            ),
          ),
        ],
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () async {
          await _showCreateAlbumDialog();
          fetchAlbums(); // Refresh albums after creation
        },
        child: const Icon(Icons.add),
      ),
    );
  }

  Future<void> _showCreateAlbumDialog() async {
    final result = await showDialog<bool>(
      context: context,
      builder: (context) => CreateAlbumDialog(
        apiService: widget.apiService,
        users: users,
      ),
    );
    if (result == true) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Album created successfully')),
      );
    }
  }
}

class DropdownMenu<T> extends StatelessWidget {
  final List<DropdownMenuItem<T>> dropdownMenuEntries;
  final T? initialSelection;
  final Widget? label;
  final ValueChanged<T?>? onSelected;

  const DropdownMenu({
    Key? key,
    required this.dropdownMenuEntries,
    this.initialSelection,
    this.label,
    this.onSelected,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return DropdownButton<T>(
      items: dropdownMenuEntries,
      value: initialSelection,
      onChanged: onSelected,
      hint: label,
    );
  }
}
