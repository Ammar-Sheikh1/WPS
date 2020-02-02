/*
  Ammar Sheikh
  Wifi Positioning System.

  I think I will eventually encapsulate this
  and put it in ammar-lib. 
*/

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sdbus-c++/sdbus-c++.h>
#include "leveldb/db.h"

#include <map>

// For sleeping
#include <thread>
#include <chrono>


int main() {
  // D-Bus Network Manager Address and Object Path
  const char* NMService = "org.freedesktop.NetworkManager";
  const char* NMObjectPath = "/org/freedesktop/NetworkManager";

  // D-Bus Object Interfaces
  const char* NMInterface = "org.freedesktop.NetworkManager";
  const char* DeviceInterface = "org.freedesktop.NetworkManager.Device";
  const char* WifiInterface = "org.freedesktop.NetworkManager.Device.Wireless";
  const char* APInterface = "org.freedesktop.NetworkManager.AccessPoint";

  // Open Database
  leveldb::DB* db;
  leveldb::Options opts;
  leveldb::ReadOptions readOpts;
  opts.create_if_missing = false;
  leveldb::Status status = leveldb::DB::Open(opts, "../db/APData", &db);
  assert(status.ok());

  // Connect to NM and get current devices
  auto nmProxy = sdbus::createProxy(NMService, NMObjectPath);
  auto deviceMethod = nmProxy->createMethodCall(NMInterface, "GetDevices");
  auto reply = nmProxy->callMethod(deviceMethod);
  std::vector<sdbus::ObjectPath> devices;
  reply >> devices;

  // Get Wifi Device and get current APs

  for (const auto& device : devices) {
    auto deviceProxy = sdbus::createProxy(NMService, device);

   // Check if Wi-Fi device -> deviceType == 2
    unsigned deviceType = 0;
    auto property = deviceProxy->getProperty("DeviceType").onInterface(DeviceInterface);
    deviceType = property.get<unsigned>();

    if (deviceType != 2) 
      continue;

    // Get all Access Points 
    auto apMethod = deviceProxy->createMethodCall(WifiInterface, "GetAllAccessPoints");

    // auto requestScan method

    auto requestScanMethod = deviceProxy->createMethodCall(WifiInterface, "RequestScan");
    std::map<std::string, sdbus::Variant> dict;
    requestScanMethod << dict;

    while (true) { 
      auto reply = deviceProxy->callMethod(apMethod);
      std::vector<sdbus::ObjectPath> accessPoints;
      reply >> accessPoints;

      // Get BSSID of each AP and check for them in DB
      double latAvg = 0;
      double lonAvg = 0;
      int noOfAPs = 0;


      for (const auto& ap : accessPoints) {
        auto apProxy = sdbus::createProxy(NMService, ap);

        auto bssidProperty = apProxy->getProperty("HwAddress").onInterface(APInterface);
        std::string bssid = bssidProperty.get<std::string>();

        // Check DB
        std::string location;
        leveldb::Status status = db->Get(readOpts, bssid, &location);

        if (status.ok()) {
          std::size_t idx = 0;
          double latitude = std::stod(location, &idx);
          double longitude = std::stod(location.substr(++idx));
          latAvg += latitude;
          lonAvg += longitude;
          ++noOfAPs;
        }
      } 

      // Request NM to continue making scans
      deviceProxy->callMethod(requestScanMethod);

      std::cout << "Estimated location: \nLatitude: " << latAvg/noOfAPs << \
      "\nLongitude: " << lonAvg/noOfAPs << "\n";
      std::cout << "NoOfAps: " << noOfAPs << "\n\n\n";

      std::this_thread::sleep_for(std::chrono::milliseconds(15000));
    }


  }
  return 0;

}
