#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <type_traits>
#include <tuple>

#include "xxhash.h"
#include <iostream>

// Type trait to check if a type is safe for bit-wise hashing
template <typename T>
struct is_safe_for_bitwise_hash
   : std::conjunction<std::is_trivially_copyable<T>, std::negation<std::is_pointer<T>>,
                      std::negation<std::is_member_pointer<T>>, std::negation<std::is_array<T>>> {};

template <typename T>
inline constexpr bool is_safe_for_bitwise_hash_v = is_safe_for_bitwise_hash<T>::value;

// Custom hash function for various types
struct hash_util {
   // Default implementation for types safe for bitwise hashing
   template <typename T>
   static typename std::enable_if_t<is_safe_for_bitwise_hash_v<T>, XXH64_hash_t> hash_element(const T&     element,
                                                                                              XXH64_hash_t seed) {
      std::aligned_storage_t<sizeof(T), alignof(T)> buffer;
      std::memcpy(&buffer, &element, sizeof(T));
      return XXH64(&buffer, sizeof(T), seed);
   }

   // Specialization for std::string
   static XXH64_hash_t hash_element(const std::string& element, XXH64_hash_t seed) {
      return XXH64(element.data(), element.size(), seed);
   }

   // Specialization for float
   static XXH64_hash_t hash_element(float element, XXH64_hash_t seed) {
      // Handle special cases like NaN and -0.0
      if (std::isnan(element)) {
         element = std::numeric_limits<float>::quiet_NaN();
      } else if (element == 0.0f) {
         element = 0.0f; // This normalizes -0.0f to 0.0f
      }
      return XXH64(&element, sizeof(float), seed);
   }

   // Specialization for double
   static XXH64_hash_t hash_element(double element, XXH64_hash_t seed) {
      // Handle special cases like NaN and -0.0
      if (std::isnan(element)) {
         element = std::numeric_limits<double>::quiet_NaN();
      } else if (element == 0.0) {
         element = 0.0; // This normalizes -0.0 to 0.0
      }
      return XXH64(&element, sizeof(double), seed);
   }

   /*// Specialization for VertexBufferLayout
   static XXH64_hash_t hash_element(const VertexBufferLayout& element, XXH64_hash_t seed) {
      auto data = element.data();
      return XXH64(data.data(), data.size() * sizeof(unsigned int), seed);
   }*/

   // Add more specializations for other types as needed
};

template <typename T>
concept Hashable = requires(T t, XXH64_hash_t seed) {
   { hash_util::hash_element(t, seed) } -> std::same_as<XXH64_hash_t>;
};

// Base case
template <typename Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct tuple_hash : hash_util {
   static XXH64_hash_t apply(const Tuple& tuple, XXH64_hash_t seed = 0) {
      XXH64_hash_t hash = tuple_hash<Tuple, Index - 1>::apply(tuple, seed);
      return hash_element(std::get<Index>(tuple), hash);
   }
};

// Specialization for index 0
template <typename Tuple>
struct tuple_hash<Tuple, 0> : hash_util {
   static XXH64_hash_t apply(const Tuple& tuple, XXH64_hash_t seed = 0) {
      return hash_element(std::get<0>(tuple), seed);
   }
};

namespace gtl {

template <typename T>
class weak_memoize_constructor {
private:
   mutable std::mutex                                         mtx;
   mutable std::unordered_map<XXH64_hash_t, std::weak_ptr<T>> cache;

   template <typename Tuple, std::size_t... Is>
   std::shared_ptr<T> construct_helper(Tuple&& args, std::index_sequence<Is...>) const {
      return std::make_shared<T>(std::get<Is>(std::forward<Tuple>(args))...);
   }

public:
   template <Hashable... Args>
   std::shared_ptr<T> operator()(Args&&... args) const {
      using KeyType = std::tuple<Args...>;

      KeyType      key(std::forward<Args>(args)...);
      XXH64_hash_t hash = tuple_hash<KeyType>::apply(key);

      std::lock_guard<std::mutex> lock(mtx);

      auto it = cache.find(hash);
      if (it != cache.end()) {
         if (auto shared_result = it->second.lock()) {
            return shared_result;
         }
      }

      // If we're here, either the key wasn't found or the weak_ptr couldn't be locked
      auto result = construct_helper(std::move(key), std::make_index_sequence<sizeof...(Args)>{});
      cache[hash] = result;
      return result;
   }

   void clear() {
      std::lock_guard<std::mutex> lock(mtx);
      cache.clear();
   }
};

template <typename T>
class global_weak_memoize_constructor {
private:
   static weak_memoize_constructor<T>& get_instance() {
      static weak_memoize_constructor<T> instance;
      return instance;
   }

public:
   template <typename... Args>
   static std::shared_ptr<T> construct(Args&&... args) {
      return get_instance()(std::forward<Args>(args)...);
   }

   static void clear() { get_instance().clear(); }
};

} // namespace gtl

// Macro for easy declaration of globally memoized constructors
#define DECLARE_GLOBAL_MEMOIZED_CONSTRUCTOR(ClassName)                                                \
   template <typename... Args>                                                                        \
   static std::shared_ptr<ClassName> create(Args&&... args) {                                         \
      return gtl::global_weak_memoize_constructor<ClassName>::construct(std::forward<Args>(args)...); \
   }                                                                                                  \
   static void clear_memoized_instances() {                                                           \
      gtl::global_weak_memoize_constructor<ClassName>::clear();                                       \
   }
