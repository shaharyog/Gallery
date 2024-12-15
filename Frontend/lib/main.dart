import 'package:dynamic_color/dynamic_color.dart';
import 'package:flutter/material.dart';
import 'package:gallery/api_service.dart';
import 'package:gallery/albums_page.dart'; // Import the AlbumsPage file

const _defaultLightColorScheme = ColorScheme.light();

const _defaultDarkColorScheme = ColorScheme.dark();

void main() {
  runApp(const GalleryApp());
}

class GalleryApp extends StatelessWidget {
  const GalleryApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return DynamicColorBuilder(
        builder: (ColorScheme? lightDynamic, ColorScheme? darkDynamic) {
      return MaterialApp(
        title: 'Gallery',
        theme: ThemeData(
            useMaterial3: true,
            colorScheme: lightDynamic ?? _defaultLightColorScheme),
        darkTheme: ThemeData(
            useMaterial3: true,
            colorScheme: darkDynamic ?? _defaultDarkColorScheme),
        themeMode: ThemeMode.system,
        home: const MyApp(),
        // Change the home to MyApp
        debugShowCheckedModeBanner: false,
      );
    });
  }
}

class MyApp extends StatefulWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  late final ApiService apiService;

  @override
  void initState() {
    super.initState();
    apiService = ApiService(baseUrl: 'http://localhost:8080/gallery/api');
  }

  @override
  Widget build(BuildContext context) {
    return AlbumsPage(apiService: apiService);
  }
}
