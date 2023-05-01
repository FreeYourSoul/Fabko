// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2023
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include "peerboard.hh"
#include <kv_rocksdb.hh>

namespace fabko {

struct peerboard::pb_impl {

  explicit pb_impl(kv_rocksdb::initializer_type&& datastore) : datastore(kv_rocksdb_instance(std::move(datastore))) {}

  kv_rocksdb_instance datastore;
};

peerboard::~peerboard() = default;

peerboard::peerboard(std::string data_store_location) : _pimpl(std::make_unique<pb_impl>(
    kv_rocksdb::initializer_type{.path_db_file = std::move(data_store_location)})) {}

void peerboard::register_capability(capability to_add) {
  //  auto serializer = serial::make_serializer();
  //  auto ser_cap = serialize(std::move(capability));
}

} // namespace fabko
