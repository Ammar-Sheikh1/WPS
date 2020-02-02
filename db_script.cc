/*Script used to dump data into levelDB database*/
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <regex>
#include <vector>
#include <cassert>


#include "includes/XMLParser.h"
#include "leveldb/db.h"

using myMap = std::unordered_map<std::string, std::pair<double, double>>;

int main(int argc, char** argv){

  if (argc < 2) {
    std::cout << "./db_script <netxml file>";
    return 0;
  }

  const char* fileName = argv[1];
  ammar::XMLParser xml(fileName);
  xml.build();

  // Parse the .netxml document for Networks, BSSID & latitude and longitude
  std::cout << "Done building tree\n";



  myMap map;

  for (const auto& network : xml.iterate("detection-run/wireless-network")) {

    std::string BSSID = "";
    double latitude = 0;
    double longitude = 0;

    for (const auto& bssid: xml.iterate("wireless-network/BSSID", network)) {
      BSSID = bssid->text;
    }
    for (const auto& lat : xml.iterate("wireless-network/gps-info/avg-lat", network)) {
      latitude = std::stod(lat->text);
    }

    for (const auto& lon : xml.iterate("wireless-network/gps-info/avg-lon", network)) {
      longitude = std::stod(lon->text);
    }
    if (BSSID.size() > 0 && latitude && longitude) {
     map[BSSID] = std::pair<double,double>(latitude, longitude);
    }
    
  } 
  

  // Open Database
  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = false;
  leveldb::Status status = leveldb::DB::Open(options, "../db/APData", &db);
  assert(status.ok());
  leveldb::WriteOptions writeopts = leveldb::WriteOptions();

  //Dump Access Point Data in DB
  int i = 1;
  for (auto iter = map.begin(), end = map.end(); iter != end; iter++) {
    std::cout << i << ": " << iter->first << "\n";
    std::cout << iter->second.first << "/" << iter->second.second << "\n";
    db->Put(writeopts, iter->first, std::to_string(iter->second.first) \
                                    + "/" + std::to_string(iter->second.second));
    ++i;
  }

  std::cout << "APs added " << i << "\n";
  // close db
  delete db;

  std::cout << "Done! \n";
  return 0;
}
