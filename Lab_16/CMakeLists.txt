# CMakeList.txt: проект CMake для Lab_16; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (Lab_16 "Lab_16.cpp" "Lab_16.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Lab_16 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
