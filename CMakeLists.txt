cmake_minimum_required(VERSION 3.5.0)
project(c64 VERSION 0.1.0 LANGUAGES C)

# Füge die Include-Verzeichnisse hinzu, falls du Header-Dateien verwendest
include_directories(${PROJECT_SOURCE_DIR}/inc)

# Alle C-Dateien in den src und lib Ordnern automatisch sammeln
file(GLOB SOURCES "main/*.c" "peripherie/*.c" "opcodes/*.c" "test/*.c" "debug/*.c")

# Die ausführbare Datei mit den gesammelten Dateien erstellen
add_executable(c64 ${SOURCES})
