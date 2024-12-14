cmake_minimum_required(VERSION 3.10)

project(smart_ptr_experiment LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# src fileのリストを変数に格納
set(SRC_LIST
  src/weak_ptr_test.cpp
)

include_directories(include)

# 実行ファイルをSRC_LISTと1対1で作成
foreach(SRC_FILE ${SRC_LIST})
  # 拡張子を除いたファイル名を取得
  get_filename_component(EXE_NAME ${SRC_FILE} NAME_WE)
  add_executable(${EXE_NAME} ${SRC_FILE})
endforeach()
