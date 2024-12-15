import 'package:intl/intl.dart';

class Album {
  final int ownerId;
  final String ownerName;
  final String name;
  final DateTime creationDate;
  final int picturesCount;

  Album({
    required this.ownerId,
    required this.ownerName,
    required this.name,
    required this.creationDate,
    required this.picturesCount,
  });

  factory Album.fromJson(Map<String, dynamic> json) {
    return Album(
      ownerId: json['owner_id'],
      name: json['name'],
      creationDate: DateTime.parse(json['creation_date']),
      picturesCount: json['pictures_count'],
      ownerName: json['owner_name'],
    );
  }

  String get formattedCreationDate => DateFormat.yMMMMd()
      .format(creationDate); // Example format: January 1, 2022
}
