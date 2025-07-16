// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#pragma once

#include <concepts>
#include <exception>
#include <memory>

#include <rocksdb/merge_operator.h>
#include <rocksdb/utilities/optimistic_transaction_db.h>
#include <rocksdb/utilities/transaction.h>

#include <common/exception.hh>

#include "key_value_db.hh"

namespace fabko {

template<typename T>
concept listing_handler = std::invocable<T, std::string&&, std::string&&>;

[[maybe_unused]] static std::error_code put_error_code() { return {0, except_cat::db {}}; }
[[maybe_unused]] static std::error_code get_error_code() { return {1, except_cat::db {}}; }
[[maybe_unused]] static std::error_code commit_error_code() { return {42, except_cat::db {}}; }

class kv_rocksdb {

  public:
    class transaction {
      public:
        explicit transaction(kv_rocksdb& db);

        bool commit_transaction();

        std::string get(const std::string& key);
        std::vector<key_value> multi_get(const std::vector<std::string>& keys);

        bool set(const key_value& to_add);
        bool multi_set(const std::vector<key_value>& to_add);

        std::unique_ptr<rocksdb::Iterator> get_iterator(const rocksdb::ReadOptions& ro = {}) {
            auto ptr = std::unique_ptr<rocksdb::Iterator>();
            ptr.reset(_transaction->GetIterator(ro));
            return ptr;
        }

        template<listing_handler Handler> void list(std::string_view start, std::string_view end_key, Handler&& handler) {
            auto it = get_iterator();
            for (it->Seek(rocksdb::Slice(start)); it->Valid(); it->Next()) {
                std::string key = it->key().ToString();
                if (key.compare(end_key) < 0) {
                    std::forward<Handler>(handler)(std::move(key), it->value().ToString());
                } else {
                    break;
                }
            }
        }

        template<listing_handler Handler> void list(std::string_view start, Handler&& handler) {
            std::string end(start);
            ++end.back();
            list(start, end, std::forward<Handler>(handler));
        }

        template<typename T> T get_as(const std::string& key) {
            std::string value = get(key);
            if constexpr (std::is_same_v<std::string, T>) {
                return value;
            } else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
                return std::stoul(value);
            } else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
                return std::stoi(value);
            } else if constexpr (std::is_floating_point_v<T>) {
                return std::stod(value);
            } else {
                throw std::logic_error("get_as not implemented");
            }
        }

        void add_counter(const std::string& key, std::int64_t to_add);

      private:
        std::unique_ptr<rocksdb::Transaction> _transaction;
    };

    struct initializer_type {
        std::string path_db_file {};
        std::vector<key_value> initial_kv {};

        //! rocksdb merge operator
        rocksdb::MergeOperator* merge_operator = nullptr;
    };

    explicit kv_rocksdb(const initializer_type& initializer);

  private:
    std::unique_ptr<rocksdb::OptimisticTransactionDB> _db;
};

using kv_rocksdb_instance = kv_db<kv_rocksdb>;

} // namespace fabko
