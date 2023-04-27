// MIT License
//
// Copyright (c) 2021 Quentin Balland
// Repository : https://github.com/FreeYourSoul/FiL
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

#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace fabko {

using key_value = std::pair<std::string, std::string>;

template<class T>
concept DbInteractivePolicy =
    requires(T v) {
      { v.get(std::string{}) } -> std::convertible_to<std::string>;
      { v.multi_get(std::vector{std::string{}}) } -> std::convertible_to<std::vector<key_value>>;
      { v.set(key_value{}) } -> std::convertible_to<bool>;
      { v.multi_set(std::vector{key_value{}}) } -> std::convertible_to<bool>;
    };

template<class T>
concept DatabasePolicy =
    requires(T v) {
      typename T::initializer_type;
    } && DbInteractivePolicy<T>;

template<typename T>
concept TransactionalDatabasePolicy =
    requires(T v) {
      typename T::transaction;
    } && DbInteractivePolicy<typename T::transaction>;

template<typename T>
class kv_db {
  static_assert(not DatabasePolicy<T>);
  static_assert(not TransactionalDatabasePolicy<T>);
};

// ******* Non-Transactional part *******

template<typename DbPolicy>
  requires DatabasePolicy<DbPolicy>
class kv_db<DbPolicy> : public DbPolicy {

public:
  explicit kv_db(const typename DbPolicy::initializer_type& initializer) : DbPolicy(initializer) {
  }

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

template<typename DbPolicy>
  requires TransactionalDatabasePolicy<DbPolicy>
class kv_db<DbPolicy> : public DbPolicy {
public:
  using transaction_type = typename DbPolicy::transaction;

  explicit kv_db(const typename DbPolicy::initializer_type& initializer) : DbPolicy(initializer) {
  }

  std::unique_ptr<transaction_type> make_transaction() {
    return std::make_unique<transaction_type>(*this);
  }
};

}// namespace fabko
