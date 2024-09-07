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

// Custom hash function for tuples using xxHash/
struct hash_util {
   template <typename T>
   static XXH64_hash_t hash_element(const T& element, XXH64_hash_t seed) {
      if constexpr (is_safe_for_bitwise_hash_v<T>) {
         std::aligned_storage_t<sizeof(T), alignof(T)> buffer;
         std::memcpy(&buffer, &element, sizeof(T));
         return XXH64(&buffer, sizeof(T), seed);
      } else if constexpr (std::is_same_v<T, std::string>) {
         return XXH64(element.data(), element.size(), seed);
      } else {
         static_assert(always_false<T>, "Unsupported type for hashing");
      }
   }

   template <typename T>
   static constexpr bool always_false = false;
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
   template <typename... Args>
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
