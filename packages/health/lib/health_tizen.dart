// You have generated a new plugin project without
// specifying the `--platforms` flag. A plugin project supports no platforms is generated.
// To add platforms, run `flutter create -t plugin --platforms <platforms> .` under the same
// directory. You can also find a detailed instruction on how to add platforms in the `pubspec.yaml` at https://flutter.dev/docs/development/packages-and-plugins/developing-packages#plugin-platforms.

import 'dart:async';

import 'package:enum_to_string/enum_to_string.dart';
import 'package:flutter/services.dart';

class HealthTizen {
  HealthTizen() {
    _channel.setMethodCallHandler(_handleMessage);
  }

  static const MethodChannel _channel = MethodChannel('health_tizen');

  static Future<String> get platformVersion async {
    final String version =
        await _channel.invokeMethod<String>('getPlatformVersion') ?? '';
    return version;
  }

  final StreamController<TizenHealthData> _streamController =
      StreamController<TizenHealthData>.broadcast();

  Stream<TizenHealthData> get stream {
    return _streamController.stream;
  }

  Future<void> start(List<TizenSensor> sensors) async {
    return _channel.invokeMethod<void>(
        'start', sensors.map(EnumToString.convertToString));
  }

  Future<void> stop() async {
    return _channel.invokeMethod<void>('stop');
  }

  Future<dynamic> _handleMessage(MethodCall call) {
    try {
      final List<String> arguments = call.arguments as List<String>;
      _streamController.add(
        TizenHealthData(
          EnumToString.fromString(TizenSensor.values, arguments[0]) ??
              TizenSensor.unknown,
          double.tryParse(arguments[1]) ?? 0,
        ),
      );
      return Future<void>.value();
    } catch (e) {
      return Future<void>.error(e);
    }
  }
}

enum TizenSensor {
  unknown,
  hrm,
  pedometer,
}

class TizenHealthData {
  TizenHealthData(this.sensor, this.value);

  final int timestamp = DateTime.now().millisecondsSinceEpoch;
  final TizenSensor sensor;
  final double value;
}
