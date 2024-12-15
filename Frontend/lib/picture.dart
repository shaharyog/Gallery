import 'package:intl/intl.dart';

class Picture {
  final int id;
  final String name;
  final String path;
  final DateTime creationDate;
  final int tagCount;

  Picture({
    required this.id,
    required this.name,
    required this.path,
    required this.creationDate,
    required this.tagCount,
  });

  factory Picture.fromJson(Map<String, dynamic> json) {
    return Picture(
      id: json['id'],
      name: json['name'],
      path: json['path'],
      creationDate: DateTime.parse(json['creation_date']),
      tagCount: json['tag_count'],
    );
  }

  String get formattedCreationDate => DateFormat.yMMMMd()
      .format(creationDate); // Example format: January 1, 2022
}
