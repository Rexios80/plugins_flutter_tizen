#include "health_tizen_plugin.h"

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>
#include <string>

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
            result->start();
        } else {
            result->NotImplemented();
        }
    }

    flutter::MethodResult<flutter::EncodableValue> start() {

    };
};

void HealthTizenPluginRegisterWithRegistrar(
        FlutterDesktopPluginRegistrarRef registrar) {
    HealthTizenPlugin::RegisterWithRegistrar(
            flutter::PluginRegistrarManager::GetInstance()
                    ->GetRegistrar<flutter::PluginRegistrar>(registrar));
}
