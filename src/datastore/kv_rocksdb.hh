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

#pragma once

#include <exception>
#include <memory>

#include <rocksdb/utilities/optimistic_transaction_db.h>
#include <rocksdb/utilities/transaction.h>
#include <rocksdb/merge_operator.h>

#include <fabko/common/exception.hh>

#include "key_value_db.hh"

// forward declarations
namespace rocksdb {
class DB;
}
//! forward declarations

namespace fil {

static std::error_code put_error_code() { return std::error_code(0, except_cat::db {}); }
static std::error_code get_error_code() { return std::error_code(1, except_cat::db {}); }
static std::error_code commit_error_code() { return std::error_code(42, except_cat::db {}); }


class kv_rocksdb {

public:
  static const bool is_transactional = true;

  class transaction {
  public:
    explicit transaction(kv_rocksdb& db);

    bool commit_transaction();

    std::string get(const std::string& key);
    std::vector<key_value> multi_get(const std::vector<std::string>& keys);

    bool set(const key_value& to_add);
    bool multi_set(const std::vector<key_value>& to_add);

    template<typename Handler>
    void list(std::string_view start, std::string_view end_key, Handler&& handler) {
      auto *it = _transaction->GetIterator(rocksdb::ReadOptions{});
      for (it->Seek(rocksdb::Slice(start)); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        if (key.compare(end_key) < 0) {
          std::forward<Handler>(handler)(key, it->value().ToString());
        }
        else {
          break;
        }
      }
    }

    template<typename Handler>
    void list(std::string_view start, Handler&& handler) {
      std::string end(start);
      ++end.back();
      list(start, end, std::forward<Handler>(handler));
    }

    template<typename T>
    T get_as(const std::string& key) {
      std::string value = get(key);
      if constexpr (std::is_same_v<std::string, T>) {
        return value;
      }
      if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
        return std::stoul(value);
      }
      if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
        return std::stoi(value);
      }
      if constexpr (std::is_floating_point_v<T>) {
        return std::stod(value);
      }
      throw std::logic_error("get_as not implemented");
    }

    void add_counter(const std::string& key, std::int64_t to_add);

  private:
    std::unique_ptr<rocksdb::Transaction> _transaction;

  };

  struct initializer_type {
    std::string path_db_file{};
    std::vector<key_value> initial_kv{};

    //! rocksdb merge operator
    rocksdb::MergeOperator* merge_operator = nullptr;

  };

  explicit kv_rocksdb(const initializer_type& initializer);

private:
  std::unique_ptr<rocksdb::OptimisticTransactionDB> _db;

};

using kv_rocksdb_type = kv_db<kv_rocksdb>;

}// namespace fil

#ifndef FIL_INCLUDE_FIL_KV_DB_KEY_VALUE_DB_HH
#define FIL_INCLUDE_FIL_KV_DB_KEY_VALUE_DB_HH

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace fil {

using key_value = std::pair<std::string, std::string>;

template<typename DbPolicy, class Enable = void>
class kv_db : public DbPolicy {};

// ******* Non-Transactional part *******

// TODO : Enforce that db has the method they should have with concept
//        It should also remove the SFINAE code and become specialization of the DbPolicy
template<typename DbPolicy>
class kv_db<DbPolicy, std::enable_if_t<!DbPolicy::is_transactional>> : public DbPolicy {

public:
  explicit kv_db(const typename DbPolicy::initializer_type& initializer) : DbPolicy(initializer) {}

  std::vector<std::string> get(const std::string& key) {
    return DbPolicy::multi_get(key);
  }

  std::vector<key_value> multi_get(const std::vector<std::string>& keys) {
    return DbPolicy::multi_get(keys);
  }

  bool set(const key_value& to_add) {
    return DbPolicy::set(to_add);
  }

  bool multi_set(const std::vector<key_value>& to_adds) {
    return DbPolicy::multi_set(to_adds);
  }

  void inc_counter(const std::string& key_counter) {
    return DbPolicy::inc_counter(key_counter);
  }

  template<typename Handler>
  void list(std::string_view start_key, std::string_view end_key, Handler&& handler) {
    DbPolicy::list(start_key, end_key, std::forward<Handler>(handler));
  }

  template<typename Handler>
  void list(std::string_view start_key, Handler&& handler) {
    std::string end(start_key);
    ++end.back();
    DbPolicy::list(start_key, end, std::forward<Handler>(handler));
  }

  template<typename T>
  T get_as(const std::string& key) {
    return DbPolicy::template get_as<T>(key);
  }
};

// ******* Transactional part *******


// TODO : Enforce that Transaction has the method they should have with concept
//        It should also remove the SFINAE code and become specialization of the DbPolicy
template<typename DbPolicy>
class kv_db<DbPolicy, std::enable_if_t<DbPolicy::is_transactional>> : public DbPolicy {
public:
  using transaction_type = typename DbPolicy::transaction;

  explicit kv_db(const typename DbPolicy::initializer_type& initializer) : DbPolicy(initializer) {}

  std::unique_ptr<transaction_type> make_transaction() {
    return std::make_unique<transaction_type>(*this);
  }
};

}// namespace fil
