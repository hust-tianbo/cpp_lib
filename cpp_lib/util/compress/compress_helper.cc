#include "cpp_lib/util/compress/compress_helper.h"

#include <zlib.h>
#define CHUNK 16384
#define windowBits 15
#define GZIP_ENCODING 16

namespace cpp_lib {
int GzipString(const std::string& data, std::string& compressData, int leval) {
  unsigned char out[CHUNK];
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  if (deflateInit2(&strm, level, Z_DEFLATED, windowBits | GZIP_ENCODING, 8,
                   Z_DEFAULT_STRATEGY) != Z_OK) {
    return -1;
  }
  strm.next_in = (unsigned char*)data.c_str();
  strm.avail_in = data.size();
  do {
    int have;
    strm.avail_out = CHUNK;
    strm.next_out = out;
    if (deflate(&strm, Z_FINISH) == Z_STREAM_ERROR) {
      return -1;
    }
    have = CHUNK - strm.avail_out;
    compressedData.append(reinterpret_cast<char*>(out), have);
  } while (strm.avail_out == 0);
  if (deflateEnd(&strm) != Z_OK) {
    return -1;
  }
  return 0;
}

int GunzipString(const std::string& compressData, std::string& data) {
  int ret;
  unsigned have;
  z_stream strm;
  unsigned char out[CHUNK];

  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK) {
    return -1;
  }

  strm.avail_in = compressedData.size();
  strm.next_in = (unsigned char*)compressedData.c_str();
  do {
    strm.avail_out = CHUNK;
    strm.next_out = out;
    ret = inflate(&strm, Z_NO_FLUSH);
    switch (ret) {
      case Z_NEED_DICT:
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
        inflateEnd(&strm);
        return -1;
    }
    have = CHUNK - strm.avail_out;
    data.append(reinterpret_cast<char*>(out), have);
  } while (strm.avail_out == 0);

  if (inflateEnd(&strm) != Z_OK) {
    return -1;
  }

  return 0;
}
}