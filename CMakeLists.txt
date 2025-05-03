cmake_minimum_required(VERSION 3.5)
project(HelloProject C)

# Configurações do compilador C
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Encontra a biblioteca ncurses
find_package(Curses REQUIRED)

# Inclui os diretórios do ncurses
include_directories(${CURSES_INCLUDE_DIR})

# Executável principal com todos os arquivos fonte
add_executable(vibeOS
  src/main.c
  src/process.c
  src/debug.c
  src/kernel.c
  src/interface.c
)

# Linka a biblioteca ncurses ao executável
target_link_libraries(vibeOS ${CURSES_LIBRARIES})


# Configurar diretórios de include
target_include_directories(vibeOS PUBLIC include/)

# Opcional: habilitar warnings
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(vibeOS PRIVATE -Wall -Wextra)
endif()