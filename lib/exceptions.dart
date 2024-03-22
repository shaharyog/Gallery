class NotFoundException implements Exception {
  final String message;

  NotFoundException(this.message);

  @override
  String toString() {
    return 'Could not found $message';
  }
}

class AlbumNotFoundException extends NotFoundException {
  AlbumNotFoundException() : super('Album');
}

class PictureNotFoundException extends NotFoundException {
  PictureNotFoundException() : super('Picture');
}

class UserNotFoundException extends NotFoundException {
  UserNotFoundException() : super('User');
}

class TagNotFoundException extends NotFoundException {
  TagNotFoundException() : super('Tag');
}

class AlreadyExistsException implements Exception {
  final String message;

  AlreadyExistsException(this.message);

  @override
  String toString() {
    return '$message Already exists: ';
  }
}

class AlbumAlreadyExistsException extends AlreadyExistsException {
  AlbumAlreadyExistsException() : super('Album');
}

class PictureAlreadyExistsException extends AlreadyExistsException {
  PictureAlreadyExistsException() : super('Picture');
}

class UserAlreadyExistsException extends AlreadyExistsException {
  UserAlreadyExistsException() : super('User');
}

class TagAlreadyExistsException extends AlreadyExistsException {
  TagAlreadyExistsException() : super('Tag');
}
