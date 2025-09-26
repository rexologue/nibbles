# Header-only библиотека: только заголовки из core/
add_library(core INTERFACE)

# Дадим потребителям путь к заголовкам: #include "scheme.h" и т.п.
target_include_directories(core
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Протащим требование C++17
target_compile_features(core INTERFACE cxx_std_17)
