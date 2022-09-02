#include <alpaca/alpaca.h>

int main() {

  struct MyStruct {
    struct gps {
      double latitude;
      double longitude;
    };
    gps location;

    struct image {
      uint16_t width;
      uint16_t height;
      std::string url;

      struct format {
        enum class type { bayer_10bit, yuyv_422 };
        type type;
      };
      format format;
    };
    image thumbnail;
  };

  MyStruct s{{41.13, -73.70},
             {480,
              340,
              "https://foo/bar/baz.jpg",
              {MyStruct::image::format::type::yuyv_422}}};

  // Serialize
  std::vector<uint8_t> bytes;
  auto bytes_written = alpaca::serialize(s, bytes); // 45 bytes

  // bytes:
  // {
  //   0x71 0x3d 0x0a 0xd7 0xa3 0x90 0x44 0x40  // double 41.13
  //   0xcd 0xcc 0xcc 0xcc 0xcc 0x6c 0x52 0xc0  // double -73.70
  //   0xe0 0x01                                // uint 480
  //   0x54 0x01                                // uint 340
  //   0x17                                     // 23-byte string
  //   0x68 0x74 0x74 0x70 0x73 0x3a 0x2f 0x2f  // "https://"
  //   0x66 0x6f 0x6f 0x2f                      // "foo/"
  //   0x62 0x61 0x72 0x2f                      // "bar/"
  //   0x62 0x61 0x7a                           // "baz"
  //   0x2e 0x6a 0x70 0x67                      // ".jpg"
  //   0x01                                     // enum value 1
  // }

  // Deserialize
  std::error_code ec;
  auto recovered = alpaca::deserialize<MyStruct>(bytes, ec);
  assert((bool)ec == false);
  assert(recovered.location.latitude == 41.13);
  assert(recovered.location.longitude == -73.70);
  assert(recovered.thumbnail.width == 480);
  assert(recovered.thumbnail.height == 340);
  assert(recovered.thumbnail.url == std::string{"https://foo/bar/baz.jpg"});
  assert(recovered.thumbnail.format.type ==
         MyStruct::image::format::type::yuyv_422);
}