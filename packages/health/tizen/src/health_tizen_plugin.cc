#include "health_tizen_plugin.h"

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <sensor.h>
#include <variant>

#include "log.h"

using namespace std;
using namespace flutter;

class HealthTizenPlugin : public Plugin {
public:
    static void RegisterWithRegistrar(PluginRegistrar *registrar) {
        auto channel =
                make_unique<MethodChannel<EncodableValue> >(
                        registrar->messenger(), "health_tizen",
                        &StandardMethodCodec::GetInstance());

        auto plugin = make_unique<HealthTizenPlugin>();

        channel->SetMethodCallHandler(
                [plugin_pointer = plugin.get()](const auto &call, auto result) {
                    plugin_pointer->HandleMethodCall(call, move(result));
                });

        registrar->AddPlugin(move(plugin));
    }

    HealthTizenPlugin() = default;

    ~HealthTizenPlugin() override = default;

private:
    void HandleMethodCall(
            const MethodCall<EncodableValue> &method_call,
            unique_ptr<MethodResult<EncodableValue> > result) {
        const auto &arguments = *method_call.arguments();

        if (method_call.method_name() == "start") {
            string error = start(arguments);
            if (error.empty()) {
                result->Success();
            } else {
                result->Error("1", error);
            }
        } else if (method_call.method_name() == "stop") {
            stop();
            result->Success();
        } else {
            result->NotImplemented();
        }
    }

    string start(const EncodableValue arguments) {
        while (holds_alternative<string>(arguments)) {
            string stringArgument = get<string>(arguments);
            if (stringArgument == "hrm") {
                startHrm();
            } else if (stringArgument == "pedometer") {
                startPedometer();
            }
        }
    }


    sensor_listener_h hrListener = sensor_listener_h();

    string startHrm() {
        sensor_type_e type = SENSOR_HRM;
        sensor_h sensor;

        bool supported;
        int error = sensor_is_supported(type, &supported);
        if (error != SENSOR_ERROR_NONE) {
            return &"sensor_is_supported error: "[error];
        }

        if (supported) {
            dlog_print(DLOG_DEBUG, LOG_TAG, "HRM is%s supported", supported ? "" : " not");
        }

        // creating an event listener
        error = sensor_create_listener(sensor, &hrListener);
        if (error != SENSOR_ERROR_NONE) {
            return &"sensor_create_listener error: %d"[error];
        }

        // Callback for sensor value change
        error = sensor_listener_set_event_cb(hrListener, 0, on_sensor_event, nullptr);
        if (error != SENSOR_ERROR_NONE) {
            return &"sensor_listener_set_event_cb error: %d"[error];
        }

        error = sensor_listener_start(hrListener);
        if (error != SENSOR_ERROR_NONE) {
            return &"sensor_listener_start error: %d"[error];
        }
    }

    static void startPedometer() {

    }

    static void on_sensor_event(sensor_h sensor, sensor_event_s *event, void *user_data) {
        // Select a specific sensor with a sensor handle
        sensor_type_e type;
        sensor_get_type(sensor, &type);

        switch (type) {
            case SENSOR_HRM:
                dlog_print(DLOG_DEBUG, LOG_TAG, "heartRate: %f", event->values[0]);
                break;
            case SENSOR_HUMAN_PEDOMETER:
                dlog_print(DLOG_DEBUG, LOG_TAG, "pedometer: %f", event->values[0]);
                break;
            default:
                dlog_print(DLOG_ERROR, LOG_TAG, "Unknown event");
        }
    }

    void stop() {
        sensor_listener_stop(hrListener);
        sensor_destroy_listener(hrListener);
    }
};

void HealthTizenPluginRegisterWithRegistrar(
        FlutterDesktopPluginRegistrarRef registrar) {
    HealthTizenPlugin::RegisterWithRegistrar(
            PluginRegistrarManager::GetInstance()
                    ->GetRegistrar<PluginRegistrar>(registrar));
}
