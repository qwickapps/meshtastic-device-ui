#pragma once

#include <stdint.h>

/**
 * @brief Rules the MUI applies when it turns Bluetooth on. Kept free of LVGL and protobuf types so they can be unit tested
 *        on the host (tests/test_BluetoothPolicy.cpp).
 */
namespace BluetoothPolicy
{

/// The firmware's public default fixed PIN (firmware DebugConfiguration.h defaultBLEPin, installed by NodeDB on factory reset)
constexpr uint32_t defaultPin = 123456;
constexpr uint32_t minPin = 100000;
constexpr uint32_t maxPin = 999999;

/**
 * @brief The ESP32 firmware starts BLE only while WiFi is not available, i.e. not (WiFi enabled and an SSID set)
 *        (firmware src/platform/esp32/main-esp32.cpp setBluetoothEnable() and isWifiAvailable()).
 */
inline bool blockedByWifi(bool wifiEnabled, const char *wifiSsid)
{
    return wifiEnabled && wifiSsid && wifiSsid[0];
}

/// A PIN that must not protect the BLE link: the publicly known default, or not six digits
inline bool isWeakPin(uint32_t pin)
{
    return pin == defaultPin || pin < minPin || pin > maxPin;
}

/**
 * @brief A random six digit PIN that is not weak. random6 returns a random value in [minPin, maxPin] (Arduino:
 *        random(minPin, maxPin + 1)); a draw that hits the default PIN is repeated.
 */
template <typename RandomFn> uint32_t randomPin(RandomFn &&random6)
{
    uint32_t pin;
    do {
        pin = (uint32_t)random6();
    } while (isWeakPin(pin));
    return pin;
}

/**
 * @brief The PIN to store when the MUI turns Bluetooth on: a stored PIN is kept (never re-randomized on a toggle), a weak
 *        one is replaced once by a random PIN, which is then stored and kept from then on.
 */
template <typename RandomFn> uint32_t pinForEnable(uint32_t storedPin, RandomFn &&random6)
{
    return isWeakPin(storedPin) ? randomPin(random6) : storedPin;
}

} // namespace BluetoothPolicy
