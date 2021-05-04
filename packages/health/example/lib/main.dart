import 'package:flutter/material.dart';
import 'package:health_tizen/health_tizen.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  _MyAppState() {
    final HealthTizen health = HealthTizen();
    health.start([TizenSensor.hrm]);
    health.stream.listen((TizenHealthReading event) {
      print('${event.sensor}: ${event.value}');
      if (event.sensor == TizenSensor.hrm) {
        setState(() {
          heartRate = event.value;
        });
      }
    });
  }

  double heartRate = 0;

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Text('Heart rate: $heartRate'),
        ),
      ),
    );
  }
}
