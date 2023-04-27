// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
//         of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
//         to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//         copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
//         copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//         AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
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

}// namespace fabko
