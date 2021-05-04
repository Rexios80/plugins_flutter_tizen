// You have generated a new plugin project without
// specifying the `--platforms` flag. A plugin project supports no platforms is generated.
// To add platforms, run `flutter create -t plugin --platforms <platforms> .` under the same
// directory. You can also find a detailed instruction on how to add platforms in the `pubspec.yaml` at https://flutter.dev/docs/development/packages-and-plugins/developing-packages#plugin-platforms.

import 'dart:async';

import 'package:enum_to_string/enum_to_string.dart';
import 'package:flutter/services.dart';
import 'package:permission_handler/permission_handler.dart';

class HealthTizen {
  HealthTizen() {
    _channel.setMethodCallHandler(_handleMessage);
  }

  static const MethodChannel _channel = MethodChannel('health_tizen');

  final StreamController<TizenHealthReading> _streamController =
      StreamController<TizenHealthReading>.broadcast();

  Stream<TizenHealthReading> get stream {
    return _streamController.stream;
  }

  Future<void> start(List<TizenSensor> sensors) async {
    final PermissionStatus status = await Permission.sensors.request();
    if (status.isDenied || status.isPermanentlyDenied) {
      return Future<void>.error('Health permissions not granted');
    }
    return _channel.invokeMethod<void>(
        'start', sensors.map(EnumToString.convertToString).toList());
  }

  Future<void> stop() async {
    return _channel.invokeMethod<void>('stop');
  }

  Future<dynamic> _handleMessage(MethodCall call) {
    try {
      final List<dynamic> arguments = call.arguments as List<dynamic>;

      _streamController.add(
        TizenHealthReading(
          EnumToString.fromString(TizenSensor.values, arguments[0]) ??
              TizenSensor.unknown,
          arguments[1] as double,
        ),
      );
      return Future<void>.value();
    } catch (e) {
      print(e);
      return Future<void>.error(e);
    }
  }
}

enum TizenSensor {
  unknown,
  hrm,
  pedometer,
}

class TizenHealthReading {
  TizenHealthReading(this.sensor, this.value);

  final int timestamp = DateTime.now().millisecondsSinceEpoch;
  final TizenSensor sensor;
  final double value;
}
