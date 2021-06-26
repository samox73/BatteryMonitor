#include <gtkmm-3.0/gtkmm.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <sstream>
#include <string>

#include "./timer.h"

template<typename T>
T StringToNumber(const std::string& numberAsString)
{
    T valor;

    std::stringstream stream(numberAsString);
    stream >> valor;
    if (stream.fail()) {
        std::runtime_error e("Failed to convert: " + numberAsString);
        throw e;
    }
    return valor;
}

void handleBatteryUpdate(const std::string& bat_energy_now, const std::string& bat_energy_full, float warning_lower_bound) {
    std::string energy_now;
    std::fstream battery_energy_now{bat_energy_now, std::ios::in};
    if(!battery_energy_now) {
        std::cout << "Could not open file '" << bat_energy_now << "'\n";
        std::exit(EXIT_FAILURE);
    }
    std::getline(battery_energy_now, energy_now);
    std::cout << bat_energy_now << ": " << energy_now << "\n";

    std::string energy_full;
    std::fstream battery_energy_full{bat_energy_full, std::ios::in};
    if(!battery_energy_full) {
        std::cout << "Could not open file '" << bat_energy_full << "'\n";
        std::exit(EXIT_FAILURE);
    }
    std::getline(battery_energy_full, energy_full);
    std::cout << bat_energy_full << ": " << energy_full << "\n";

    float current_energy_ratio{StringToNumber<float>(energy_now)/StringToNumber<float>(energy_full)};
    std::cout << "Battery status: " << current_energy_ratio * 100 << "%\n";
    if(current_energy_ratio < warning_lower_bound) {
        auto Application = Gio::Application::create("hello.world", Gio::APPLICATION_FLAGS_NONE);
        Application->register_application();
        auto Notification = Gio::Notification::create("Battery low!");
        Notification->set_body("Please connect to power source to prevent loss of data.");
        auto Icon = Gio::ThemedIcon::create("dialog-information");
        Notification->set_icon (Icon);
        Application->send_notification(Notification);
    }
}

int main() {
    const std::string BAT_ENERGY_NOW{"/sys/class/power_supply/BAT0/energy_now"};
    const std::string BAT_ENERGY_FULL{"/sys/class/power_supply/BAT0/energy_full"};
    const std::string BAT_STATUS{"/sys/class/power_supply/BAT0/status"};
    constexpr float WARNING_LOWER_BOUND{0.10f};
    constexpr size_t pollingPeriodInMillis{10000};


    Timer<std::string, std::string, float> timer{handleBatteryUpdate, pollingPeriodInMillis};

    std::cout << "BAT NOW: " << BAT_ENERGY_NOW << std::endl;
    std::cout << "BAT FULL: " << BAT_ENERGY_FULL << std::endl;

    timer.start(BAT_ENERGY_NOW, BAT_ENERGY_FULL, WARNING_LOWER_BOUND);
    timer.join();

    return 0;
}
