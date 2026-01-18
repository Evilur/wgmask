#pragma once

#define PREVENT_COPY_ALLOW_MOVE(T)                                            \
T(const T&) = delete;                                                         \
T& operator=(const T&) = delete;                                              \
T(T&&) noexcept = default;                                                    \
T& operator=(T&&) noexcept = default

#define PREVENT_COPY_AND_MOVE(T)                                              \
T(const T&) = delete;                                                         \
T& operator=(const T&) = delete;                                              \
T(T&&) = delete;                                                              \
T& operator=(T&&) = delete

#define PREVENT_INSTANTIATION(T)                                              \
T() = delete;                                                                 \
~T() = default;                                                               \
T(const T&) = delete;                                                         \
T& operator=(const T&) = delete;                                              \
T(T&&) = delete;                                                              \
T& operator=(T&&) = delete
