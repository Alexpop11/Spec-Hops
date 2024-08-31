#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <type_traits>
#include <tuple>

namespace gtl {

// Custom hash function for tuples
template <typename Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct tuple_hash {
   static size_t apply(const Tuple& tuple) {
      size_t seed = tuple_hash<Tuple, Index - 1>::apply(tuple);
      return seed ^ (std::hash<typename std::tuple_element<Index, Tuple>::type>{}(std::get<Index>(tuple)) + 0x9e3779b9 +
                     (seed << 6) + (seed >> 2));
   }
};

template <typename Tuple>
struct tuple_hash<Tuple, 0> {
   static size_t apply(const Tuple& tuple) {
      return std::hash<typename std::tuple_element<0, Tuple>::type>{}(std::get<0>(tuple));
   }
};

template <typename T>
class weak_memoize_constructor {
private:
   mutable std::mutex                                   mtx;
   mutable std::unordered_map<size_t, std::weak_ptr<T>> cache;

   template <typename Tuple, std::size_t... Is>
   std::shared_ptr<T> construct_helper(Tuple&& args, std::index_sequence<Is...>) const {
      return std::make_shared<T>(std::get<Is>(std::forward<Tuple>(args))...);
   }

public:
   template <typename... Args>
   std::shared_ptr<T> operator()(Args&&... args) const {
      using KeyType = std::tuple<typename std::decay<Args>::type...>;

      KeyType key(std::forward<Args>(args)...);
      size_t  hash = tuple_hash<KeyType>::apply(key);

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
