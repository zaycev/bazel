// Copyright 2016 The Bazel Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef BAZEL_SRC_TOOLS_SINGLEJAR_MAPPED_FILE_H_
#define BAZEL_SRC_TOOLS_SINGLEJAR_MAPPED_FILE_H_ 1

#include <string>

/*
 * A mapped read-only file with auto closing.
 *
 * MappedFile::Open maps a file with specified name to memory as read-only.
 * It is assumed that the address space is large enough for that.
 * MappedFile::Close deletes the mapping. The destructor calls it, too.
 * A predictable set of methods provide conversion between file offsets and
 * mapped addresses, returns map size, etc.
 */
class MappedFile {
 public:
#ifdef _WIN32
  typedef /* HANDLE = void* */ void *FileHandleType;
#else   // not _WIN32
  typedef int FileHandleType;
#endif  // _WIN32

  MappedFile();

  ~MappedFile() { Close(); }

  bool Open(const std::string &path);

  void Close();

  bool mapped(const void *addr) const {
    return mapped_start_ <= addr && addr < mapped_end_;
  }

  const unsigned char *start() const { return mapped_start_; }
  const unsigned char *end() const { return mapped_end_; }
  const unsigned char *address(off_t offset) const {
    return mapped_start_ + offset;
  }
  off_t offset(const void *address) const {
    return reinterpret_cast<const unsigned char *>(address) - mapped_start_;
  }
  FileHandleType fd() const { return fd_; }
  size_t size() const { return mapped_end_ - mapped_start_; }
  bool is_open() const;

 private:
  unsigned char *mapped_start_;
  unsigned char *mapped_end_;
  FileHandleType fd_;
};

#ifdef _WIN32
#include "src/tools/singlejar/mapped_file_windows.inc"
#else  // not _WIN32
#include "src/tools/singlejar/mapped_file_posix.inc"
#endif  // _WIN32

#endif  //  BAZEL_SRC_TOOLS_SINGLEJAR_MAPPED_FILE_H_
