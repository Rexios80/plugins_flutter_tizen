import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:health_tizen/health_tizen.dart';

void main() {
  const MethodChannel channel = MethodChannel('health_tizen');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(await HealthTizen.platformVersion, '42');
  });
}
