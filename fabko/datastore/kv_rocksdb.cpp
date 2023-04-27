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

#include <optional>

#include <rocksdb/options.h>
#include <rocksdb/utilities/optimistic_transaction_db.h>
#include <rocksdb/utilities/transaction.h>

#include <fmt/format.h>

#include "kv_rocksdb.hh"
#include <exception.hh>

namespace fabko {

kv_rocksdb::transaction::transaction(kv_rocksdb& db) {
  rocksdb::WriteOptions write_options;
  rocksdb::Transaction* txn = db._db->BeginTransaction(write_options);
  txn->SetSnapshot();
  _transaction.reset(txn);
}

std::string kv_rocksdb::transaction::get(const std::string& key) {
  std::string result;
  rocksdb::ReadOptions opt;

  auto s = _transaction->Get(opt, key, &result);
  fabko_assert(
      s.ok(), get_error_code(), fmt::format("Error while getting key {} : {}", key, s.ToString()));
  return result;
}

std::vector<key_value> kv_rocksdb::transaction::multi_get(const std::vector<std::string>& keys) {
  std::vector<key_value> results;
  std::vector<std::string> r;
  rocksdb::ReadOptions opt;
  std::vector<rocksdb::Slice> slicing;

  slicing.reserve(keys.size());
  for (const auto& key : keys) {
    slicing.emplace_back(key);
  }
  auto status = _transaction->MultiGet(opt, slicing, &r);

  results.reserve(r.size());
  for (std::size_t i = 0; i < status.size(); ++i) {
    fabko_assert(
        status.at(i).ok(), get_error_code(), fmt::format("Error while multi getting {} values : {}", keys.size(), status.at(i).ToString()));
    results.emplace_back(keys.at(i), r.at(i));
  }
  return results;
}

bool kv_rocksdb::transaction::set(const key_value& to_add) {
  std::string value;
  rocksdb::Status s = _transaction->Put(to_add.first, to_add.second);
  fabko_assert(
      s.ok(), put_error_code(), fmt::format("Error while inserting key-value : {}-{} : {}", to_add.first, to_add.second, s.ToString()));
  return true;
}

bool kv_rocksdb::transaction::multi_set(const std::vector<key_value>& to_adds) {
  for (const auto& to_add : to_adds) {
    set(to_add);
  }
  return true;
}

bool kv_rocksdb::transaction::commit_transaction() {
  auto s = _transaction->Commit();
  fabko_assert(
      s.ok(), commit_error_code(), fmt::format("Commit Failure : {}", s.ToString()));
  return true;
}

void kv_rocksdb::transaction::add_counter(const std::string& /*key*/, std::int64_t /*to_add*/){}

kv_rocksdb::kv_rocksdb(const initializer_type& initializer) {

  rocksdb::OptimisticTransactionDB* txn_db;
  const rocksdb::Options option = [&initializer]() {
    rocksdb::Options opt;
    opt.create_if_missing = true;
    if (initializer.merge_operator) {
      opt.merge_operator.reset(initializer.merge_operator);
    }
    return opt;
  }();
  auto status = rocksdb::OptimisticTransactionDB::Open(option, initializer.path_db_file, &txn_db);

  fabko_assert(
      status.ok(), std::error_code{}, fmt::format(FMT_STRING("Couldn't open optimistic RocksDB Database: {}"), status.ToString()));

  _db.reset(txn_db);

  for (auto& [k, v] : initializer.initial_kv) {
    _db->Put(rocksdb::WriteOptions(), k, v);
  }
}

}// namespace fabko
