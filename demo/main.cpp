#include <structbyte/structbyte.h>

struct MyStruct {
  char a;
  int b;
  float c;
  unsigned e;
  bool f;

  enum class color { red, blue, green };
  color g;
};

int main() {

  MyStruct s{'a', 5, 3.14f, 12345, true, MyStruct::color::green};

  // Serialize
  auto bytes = structbyte::serialize(s); // 10 bytes
  std::cout << bytes.size() << "\n";
  structbyte::detail::print_bytes(bytes);

  // Deserialize
  auto recovered = structbyte::deserialize<MyStruct>(bytes);
}
