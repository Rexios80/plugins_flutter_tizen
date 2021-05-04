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
        auto plugin = make_unique<HealthTizenPlugin>(registrar);
        registrar->AddPlugin(move(plugin));
    }

    explicit HealthTizenPlugin(PluginRegistrar *registrar) {
        auto channel =
                make_unique<MethodChannel<EncodableValue> >(
                        registrar->messenger(), "health_tizen",
                        &StandardMethodCodec::GetInstance());

        channel->SetMethodCallHandler(
                [plugin_pointer = this](const auto &call, auto result) {
                    plugin_pointer->HandleMethodCall(call, move(result));
                });

        channel_ = std::move(channel);
    }

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
        string error = "";
        if (holds_alternative<EncodableList>(arguments)) {
            const EncodableList argumentList = get<EncodableList>(arguments);
            for (int i = 0; i < argumentList.size(); i++) {
                string stringArgument = get<string>(argumentList[i]);
                if (stringArgument == "hrm") {
                    error += "\n" + startHrm();
                } else if (stringArgument == "pedometer") {
                    error += "\n" + startPedometer();
                }
            }
        }

        return error;
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

        return "";
    }

    string startPedometer() {
        return "";
    }

    static void on_sensor_event(sensor_h sensor, sensor_event_s *event, void *user_data) {
        // Select a specific sensor with a sensor handle
        sensor_type_e type;
        sensor_get_type(sensor, &type);

        switch (type) {
            case SENSOR_HRM: {
                dlog_print(DLOG_DEBUG, LOG_TAG, "heartRate: %f", event->values[0]);
                auto arguments = make_unique<EncodableValue>(EncodableValue(event->values[0]));
                channel_->InvokeMethod("dataReceived", move(arguments));
                break;
            }
            case SENSOR_HUMAN_PEDOMETER: {
                dlog_print(DLOG_DEBUG, LOG_TAG, "pedometer: %f", event->values[0]);
                break;
            }
            default: {
                dlog_print(DLOG_ERROR, LOG_TAG, "Unknown event");
            }
        }
    }

    void stop() {
        sensor_listener_stop(hrListener);
        sensor_destroy_listener(hrListener);
    }

    static unique_ptr<MethodChannel<EncodableValue>> channel_;
};

unique_ptr<MethodChannel<EncodableValue>> HealthTizenPlugin::channel_;

void HealthTizenPluginRegisterWithRegistrar(
        FlutterDesktopPluginRegistrarRef registrar) {
    HealthTizenPlugin::RegisterWithRegistrar(
            PluginRegistrarManager::GetInstance()
                    ->GetRegistrar<PluginRegistrar>(registrar));
}
