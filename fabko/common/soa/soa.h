// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by fys on 17.06.25. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#ifndef SOA_H
#define SOA_H

#include <algorithm>
#include <cstdint>
#include <functional>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace fil {

//
// to be moved in a metaprog utility of fil
//

/**
 * @brief Concept to check if a type is nothrow move constructible and nothrow move assignable.
 */
template<typename T>
concept nothrow_movable = std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>;

/**
 * @brief retrieve the index of the type T in the parameter pack Ts.
 * @details first parameter_pack_index<typename T, typename, typename... Ts> parameter_pack_index unpack the
 * parameter pack Ts until it triggers the specialization parameter_pack_index<T, T, Ts...> which forces the
 * two first types to be the same, thus returning stopping the parameter pack iteration.
 */
template<typename T, typename, typename... Ts> static constexpr std::size_t parameter_pack_index    = 1 + parameter_pack_index<T, Ts...>;
template<typename T, typename... Ts> static constexpr std::size_t parameter_pack_index<T, T, Ts...> = 0;

template<typename T> class soa_struct_t {
    friend T;

  public:
    [[nodiscard]] typename T::struct_id struct_id() const;

    template<std::size_t index> [[nodiscard]] friend auto& get(soa_struct_t ss) { return std::get<index>(ss.soa_->data_)[ss.offset_]; }

  private:
    soa_struct_t(T* soa, std::size_t offset)
        : soa_(soa)
        , offset_(offset) {}

    T* soa_;
    std::size_t offset_;
};

/**
 * @brief Class representing a structure of arrays (soa) where multiple vectors
 * store the individual components of a structure, enabling efficient processing
 * of large datasets.
 *
 * @tparam struct_types Variadic template parameter pack representing the types
 * of elements present in each structure.
 */
template<nothrow_movable... struct_types> class soa {

  public:
    //! number of elements in the structure handled by the soa class
    static constexpr std::size_t struct_types_size = sizeof...(struct_types);

    //! retrieve the type at the provided index
    template<std::size_t index> using struct_type_at = std::tuple_element_t<index, std::tuple<struct_types...>>;

    /**
     * @brief id of a structure in the soa. A structure is composed of each element of the soa class.
     * It is possible to retrieve partially a structure by selecting its element through the select utility free function
     *
     * @note the class is equivalent to a std::uint32_t in size and thus is trivially copiable. It is advised to use
     * this structure with value semantic.
     */
    struct struct_id;

    /**
     * @brief iterator class following c++ standard.
     * This enables the usage of ranges and standard algorithm with the soa class
     */
    template<typename> class iterator_t;

    struct sentinel {};                           //!< sentinel used for end iteration over a soa
    using iterator       = iterator_t<soa>;       //!< iterator declaration following standard
    using const_iterator = iterator_t<const soa>; //!< iterator declaration following standard

    using struct_number = std::index_sequence_for<struct_types...>;

  private:
    //! setup soa_struct as a friend class to provide the ability for soa class to instantiate it
    template<typename> friend class soa_struct_t;
    using soa_struct       = soa_struct_t<soa>;
    using const_soa_struct = soa_struct_t<const soa>;

  public:
    template<typename... Us> [[nodiscard]] struct_id insert(Us... us);

    /**
     * @brief erase an element via a specific structure id
     * @param id structure id to be removed from the soa class
     * @return true if erase succeeded, false otherwise
     */
    bool erase(struct_id id);

    /**
     * @brief ease an element via a provided iterator
     * @param it iterator designating the element to remove from the soa class
     * @return true if erase succeeded, false otherwise
     */
    bool erase(const_iterator it);

    /**
     * @brief reserve memory for each vector of the soa
     * @param size to reserve space
     */
    void reserve(std::size_t size);

    [[nodiscard]] soa_struct operator[](struct_id);
    [[nodiscard]] const_soa_struct operator[](struct_id) const;

    // [[nodiscard]] const_row operator[](row_id k) const;

    /**
     * @param id to check if the structure has
     * @return true if the structure of array contains the provided id
     */
    [[nodiscard]] bool has_id(struct_id id) const;
    /**
     * @return true if the soa structure is empty of any data
     */
    [[nodiscard]] bool is_empty() const { return reverse_indexes_.empty(); }
    /**
     * @return size o the soa structure
     */
    [[nodiscard]] std::size_t size() const { return reverse_indexes_.size(); }
    /**
     * @note standard enforced api for iterator
     * @return begin iterator on the sao structure
     */
    [[nodiscard]] iterator begin();
    /**
     * @note standard enforced api for iterator
     * @return begin iterator on the sao structure
     */
    [[nodiscard]] const_iterator begin() const;
    /**
     * @note standard enforced api for iterator
     * @return begin iterator on the sao structure
     */
    [[nodiscard]] const_iterator cbegin() const;

    /**
     * @note standard enforced api for iterator
     * @return end iterator (aka sentinel in the case of soa)
     */
    [[nodiscard]] sentinel end() const;
    /**
     * @note standard enforced api for iterator
     * @return end iterator (aka sentinel in the case of soa) as constant
     */
    [[nodiscard]] sentinel cend() const;

  private:
    std::tuple<std::vector<struct_types>...> data_;
    std::vector<struct_id> reverse_indexes_;
    std::vector<struct_id> indexes_;
    struct_id next_free_index_ {0, 0};
};

template<nothrow_movable... struct_types> template<typename T> class soa<struct_types...>::iterator_t {
    friend class soa;
    friend class soa_struct_t<soa>;

  public:
    using value_type      = soa_struct_t<T>;
    using difference_type = std::ptrdiff_t;

    iterator_t() = default;

    bool operator==(const iterator_t& t) const noexcept { return data_ == t.data_ && offset_ == t.offset_; }
    bool operator==(sentinel) const noexcept { return offset_ == data_->size(); }
    value_type operator*() const noexcept { return (*data_)[data_->reverse_indexes_[offset_]]; }
    value_type operator*() noexcept { return (*data_)[data_->reverse_indexes_[offset_]]; }

    iterator_t& operator++() noexcept {
        ++offset_;
        return *this;
    }

    iterator_t operator++(int) noexcept {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

  private:
    iterator_t(T* soa, std::size_t offset)
        : data_(soa)
        , offset_(offset) {}

    T* data_       = nullptr;
    size_t offset_ = 0;
};

// ------------------------------------------------------------------------------------------------------------------------------------
//
// Implementation side of the soa class
//
// This section contains all implementation details of the interface defined above of the soa class
//
//

template<nothrow_movable... struct_types> struct soa<struct_types...>::struct_id {
    struct_id next_generation() const noexcept;

    explicit constexpr struct_id(std::uint32_t i, std::uint8_t g = 0) noexcept;

    bool operator==(const struct_id&) const = default;

    std::uint32_t offset : 24;
    std::uint8_t generation;
};

template<nothrow_movable... struct_types>
constexpr soa<struct_types...>::struct_id::struct_id(std::uint32_t i, std::uint8_t g) noexcept
    : offset(i)
    , generation(g) {}

template<typename T> typename T::struct_id soa_struct_t<T>::struct_id() const { return soa_->reverse_indexes_[offset_]; }

template<nothrow_movable... struct_types> template<typename... Us> soa<struct_types...>::struct_id soa<struct_types...>::insert(Us... us) {

    std::invoke(
        [this]<std::size_t... Is, typename T>(std::index_sequence<Is...>, T to_insert) {
            (std::get<Is>(data_).emplace_back(std::get<Is>(std::forward<T>(to_insert))), ...);
        },
        struct_number {},
        std::forward_as_tuple(std::forward<Us>(us)...));

    if (next_free_index_.offset >= indexes_.size()) {
        indexes_.emplace_back(next_free_index_);
        reverse_indexes_.emplace_back(next_free_index_);
        next_free_index_ = struct_id {static_cast<std::uint32_t>(reverse_indexes_.size()), 0};
        return indexes_.back();
    }

    auto free_index_offset = next_free_index_.offset;
    reverse_indexes_.push_back(next_free_index_);
    next_free_index_                   = indexes_[free_index_offset];
    indexes_[free_index_offset].offset = reverse_indexes_.size() - 1;

    return indexes_[next_free_index_.offset];
}
// soa definition
//
template<nothrow_movable... struct_types> bool soa<struct_types...>::erase(struct_id id) {
    if (!has_id(id))
        return false;

    const auto id_removed  = id.offset;
    const auto offset_data = reverse_indexes_[id_removed].offset;

    auto swap_all_data = [this, offset_data]<std::size_t... Is>(std::index_sequence<Is...>) {
        auto swap_one_vec = [this, offset_data]<typename T>(T& vec) { //
            std::swap(vec[offset_data], vec.back());
            vec.pop_back();
        };
        (std::invoke(swap_one_vec, std::get<Is>(data_)), ...);
    };
    std::invoke(swap_all_data, struct_number {});

    std::swap(reverse_indexes_[id_removed], reverse_indexes_.back());
    indexes_[reverse_indexes_.back().offset]             = next_free_index_;
    indexes_[reverse_indexes_[id_removed].offset].offset = id_removed;
    next_free_index_                                     = reverse_indexes_.back();
    ++reverse_indexes_.back().generation;
    reverse_indexes_.pop_back();
    return true;
}
template<nothrow_movable... struct_types> bool soa<struct_types...>::erase(const_iterator it) { return erase(*it); }
template<nothrow_movable... struct_types> void soa<struct_types...>::reserve(std::size_t size) {
    std::invoke([this, size]<std::size_t... Is>(std::index_sequence<Is...>) { (std::get<Is>(data_).reserve(size), ...); },
                struct_number {});

    indexes_.reserve(size);
    reverse_indexes_.reserve(size);
    next_free_index_ = struct_id {static_cast<std::uint32_t>(size), 0};
}
template<nothrow_movable... struct_types> soa_struct_t<soa<struct_types...>> soa<struct_types...>::operator[](struct_id k) {
    return {this, indexes_[k.offset].offset};
}
template<nothrow_movable... struct_types>
typename soa<struct_types...>::const_soa_struct soa<struct_types...>::operator[](struct_id k) const {
    return {this, indexes_[k.offset].offset};
}
template<nothrow_movable... struct_types> bool soa<struct_types...>::has_id(struct_id id) const {
    return std::ranges::any_of(reverse_indexes_, [id](auto& i) { return i == id; });
}
template<nothrow_movable... struct_types> typename soa<struct_types...>::iterator soa<struct_types...>::begin() { return {this, 0}; }
template<nothrow_movable... struct_types> typename soa<struct_types...>::const_iterator soa<struct_types...>::begin() const {
    return {this, 0};
}
template<nothrow_movable... struct_types> typename soa<struct_types...>::const_iterator soa<struct_types...>::cbegin() const {
    return {this, 0};
}
template<nothrow_movable... struct_types> typename soa<struct_types...>::sentinel soa<struct_types...>::end() const { return {}; }
template<nothrow_movable... struct_types> typename soa<struct_types...>::sentinel soa<struct_types...>::cend() const { return {}; }

// ------------------------------------------------------------------------------------------------------------------------------------
// utility functions

constexpr auto soa_apply = []<typename F>(F&& func) {
    return [&]<typename T>(T&& t) {                                                                                                 //
        auto apply_on_struct = [f = std::forward<F>(func), t = std::forward<T>(t)]<std::size_t... Is>(std::index_sequence<Is...>) { //
            return std::invoke(f, get<Is>(t)...);
        };

        return std::invoke(apply_on_struct, std::make_index_sequence<std::tuple_size_v<T>> {});
    };
};

template<std::size_t... Ts>
requires(sizeof...(Ts) > 0)
auto select(auto&& func) {
    return [&]<typename T>(T&& t) { return std::invoke(func, get<Ts>(t)...); };
};

} // namespace fil

//
// specialization tuple operation for sao_struct_t
namespace std {

template<typename T> struct tuple_size<fil::soa_struct_t<T>> : std::integral_constant<std::size_t, T::struct_types_size> {};

// Add this before the other specializations
template<typename T> struct tuple_size<fil::soa_struct_t<T>&> : tuple_size<fil::soa_struct_t<T>> {};

template<typename T> struct tuple_size<const fil::soa_struct_t<T>> : tuple_size<fil::soa_struct_t<T>> {};

template<typename T> struct tuple_size<const fil::soa_struct_t<T>&> : tuple_size<fil::soa_struct_t<T>> {};

template<std::size_t I, typename T> struct tuple_element<I, fil::soa_struct_t<T>> {
    using type = typename T::template struct_type_at<I>;
};

} // namespace std

#endif // SOA_H
