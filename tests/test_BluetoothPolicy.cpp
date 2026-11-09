#include <doctest/doctest.h>

#include "graphics/common/BluetoothPolicy.h"

#include <vector>

namespace
{
// deterministic random source: returns the given values in order
struct Sequence {
    std::vector<long> values;
    size_t next = 0;
    long operator()() { return values.at(next++); }
};
} // namespace

TEST_CASE("BluetoothPolicy: WiFi keeps BLE from starting only when enabled with an SSID")
{
    CHECK(BluetoothPolicy::blockedByWifi(true, "home"));
    CHECK_FALSE(BluetoothPolicy::blockedByWifi(true, ""));
    CHECK_FALSE(BluetoothPolicy::blockedByWifi(true, nullptr));
    CHECK_FALSE(BluetoothPolicy::blockedByWifi(false, "home"));
}

TEST_CASE("BluetoothPolicy: the public default PIN and non six digit PINs are weak")
{
    CHECK(BluetoothPolicy::isWeakPin(123456));
    CHECK(BluetoothPolicy::isWeakPin(0));
    CHECK(BluetoothPolicy::isWeakPin(99999));
    CHECK(BluetoothPolicy::isWeakPin(1000000));
    CHECK_FALSE(BluetoothPolicy::isWeakPin(100000));
    CHECK_FALSE(BluetoothPolicy::isWeakPin(999999));
    CHECK_FALSE(BluetoothPolicy::isWeakPin(482913));
}

TEST_CASE("BluetoothPolicy: enabling keeps a stored PIN and never draws a new one")
{
    Sequence rng{{111111}};
    CHECK(BluetoothPolicy::pinForEnable(482913, rng) == 482913);
    CHECK(rng.next == 0);
}

TEST_CASE("BluetoothPolicy: enabling replaces the default PIN with a random one")
{
    Sequence rng{{654321}};
    CHECK(BluetoothPolicy::pinForEnable(BluetoothPolicy::defaultPin, rng) == 654321);
    CHECK(rng.next == 1);

    Sequence unset{{700001}};
    CHECK(BluetoothPolicy::pinForEnable(0, unset) == 700001);
}

TEST_CASE("BluetoothPolicy: a random draw that hits the default PIN is repeated")
{
    Sequence rng{{123456, 123456, 345678}};
    CHECK(BluetoothPolicy::randomPin(rng) == 345678);
    CHECK(rng.next == 3);
}
