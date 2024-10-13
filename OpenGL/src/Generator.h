// Generator.h
#pragma once
#include <coroutine>
#include <optional>
#include <memory>
#include <variant>
#include <exception>

template <typename T = void>
struct Generator {
   struct promise_type;
   using handle_type = std::coroutine_handle<promise_type>;

   struct promise_type {
      std::variant<std::monostate, int> wait_for_frames; // For advanced waiting

      Generator get_return_object() { return Generator{handle_type::from_promise(*this)}; }

      std::suspend_always initial_suspend() { return {}; }

      std::suspend_always final_suspend() noexcept { return {}; }

      std::suspend_always yield_value(int frames) { // For waiting frames
         wait_for_frames = frames;
         return {};
      }

      void return_void() {}

      void unhandled_exception() { std::terminate(); }
   };

   handle_type coro;

   Generator(handle_type h)
      : coro(h) {}

   Generator(const Generator&)            = delete;
   Generator& operator=(const Generator&) = delete;

   Generator(Generator&& other) noexcept
      : coro(other.coro) {
      other.coro = nullptr;
   }

   Generator& operator=(Generator&& other) noexcept {
      if (this != &other) {
         if (coro)
            coro.destroy();
         coro       = other.coro;
         other.coro = nullptr;
      }
      return *this;
   }

   ~Generator() {
      if (coro)
         coro.destroy();
   }

   bool move_next() {
      if (!coro.done())
         coro.resume();
      return !coro.done();
   }

   std::variant<std::monostate, int>& wait_for_frames() { return coro.promise().wait_for_frames; }
};
