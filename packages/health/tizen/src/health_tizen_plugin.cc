#include "health_tizen_plugin.h"

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <sensor.h>

#include "log.h"

class HealthTizenPlugin : public flutter::Plugin {
public:
    static void RegisterWithRegistrar(flutter::PluginRegistrar *registrar) {
        auto channel =
                std::make_unique<flutter::MethodChannel<flutter::EncodableValue> >(
                        registrar->messenger(), "health_tizen",
                        &flutter::StandardMethodCodec::GetInstance());

        auto plugin = std::make_unique<HealthTizenPlugin>();

        channel->SetMethodCallHandler(
                [plugin_pointer = plugin.get()](const auto &call, auto result) {
                    HealthTizenPlugin::HandleMethodCall(call, std::move(result));
                });

        registrar->AddPlugin(std::move(plugin));
    }

    HealthTizenPlugin() = default;

    ~HealthTizenPlugin() override = default;

private:
    static void HandleMethodCall(
            const flutter::MethodCall<flutter::EncodableValue> &method_call,
            std::unique_ptr<flutter::MethodResult<flutter::EncodableValue> > result) {
        // Replace "getPlatformVersion" check with your plugin's method.
        if (method_call.method_name() == "start") {
            std::string error = start(*method_call.arguments());
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

    static std::string start(flutter::EncodableValue arguments) {
        std::vector<std::string> stringArguments = std::get<std::vector<std::string>>(arguments);
        for (auto &stringArgument : stringArguments) {
            if (stringArgument == "hrm") {
                startHrm();
            } else if (stringArgument == "pedometer") {
                startPedometer();
            }
        }
    }

    static sensor_listener_h hrListener;

    static std::string startHrm() {
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

    static void stop() {
        sensor_listener_stop(hrListener);
        sensor_destroy_listener(hrListener);
    }
};

void HealthTizenPluginRegisterWithRegistrar(
        FlutterDesktopPluginRegistrarRef registrar) {
    HealthTizenPlugin::RegisterWithRegistrar(
            flutter::PluginRegistrarManager::GetInstance()
                    ->GetRegistrar<flutter::PluginRegistrar>(registrar));
}
