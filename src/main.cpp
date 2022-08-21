#include <iostream>
#include <serialize/serialize.h>

struct my_struct {
  bool flag;
  std::string s;
  int i;
  float f;
  std::vector<int> list;
  std::vector<std::vector<char>> list_of_lists;
  std::vector<std::vector<int>> list_of_lists_int;
};

int main() {
  std::ios_base::fmtflags f(std::cout.flags());

  // Test 1
  {
    my_struct s{true,
                {"Hello world!"},
                5,
                3.14,
                {1, 2, 3, 4, 5},
                {{'a', 'b', 'c'}, {'d', 'e', 'f'}},
                {{123, 456, 789}, {101112, 131415, 161718}}};
    std::cout << "Original struct size : " << sizeof(s) << " bytes\n";

    std::vector<uint8_t> bytes{};
    serialize<my_struct>(s, bytes);

    bytes.shrink_to_fit();
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 2
  {
    struct list {
      std::vector<int> values;
    };
    list s;
    for (int i = 0; i < 1E6; ++i) {
      s.values.push_back(i);
    }

    std::vector<uint8_t> bytes{};
    serialize<list>(s, bytes);
    bytes.shrink_to_fit();
    std::cout << "Original struct size : "
              << sizeof(s) + s.values.size() * sizeof(s.values[0])
              << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s) + s.values.size() * sizeof(s.values[0])) /
                  float(bytes.size()) * 100.0f)
              << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 3
  {
    struct my_struct {
      const char *str;
    };
    my_struct s;
    s.str = "This is cool";

    std::vector<uint8_t> bytes{};
    serialize<my_struct>(s, bytes);
    bytes.shrink_to_fit();
    std::cout << "Original struct size : "
              << sizeof(s) + strlen(s.str) * sizeof(s.str[0]) << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s) + strlen(s.str) * sizeof(s.str[0])) /
                  float(bytes.size()) * 100.0f)
              << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 4
  {
    struct my_struct {
      std::vector<bool> values;
    };
    my_struct s;
    s.values = {true, false, true, false, true};

    std::vector<uint8_t> bytes{};
    serialize<my_struct>(s, bytes);
    bytes.shrink_to_fit();
    std::cout << "Original struct size : " << sizeof(s) << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 5
  {
    struct my_struct {
      std::tuple<bool, int, float, std::string, char> values;
    };
    my_struct s;
    s.values = std::make_tuple(true, 5, 3.14, "Hello", 'a');

    std::vector<uint8_t> bytes{};
    serialize<my_struct>(s, bytes);
    bytes.shrink_to_fit();
    std::cout << "Original struct size : " << sizeof(s) << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 6
  {
    struct my_struct {
      std::vector<std::tuple<bool, int, float, std::string, char>> values;
    };
    my_struct s;
    s.values.push_back(std::make_tuple(true, 5, 3.14, "Hello", 'a'));
    s.values.push_back(std::make_tuple(false, -15, 2.718, "World", 'z'));

    std::vector<uint8_t> bytes{};
    serialize<my_struct>(s, bytes);
    bytes.shrink_to_fit();
    std::cout << "Original struct size : " << sizeof(s) << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 7
  {
    struct my_struct {
      std::pair<int, float> values;
    };
    my_struct s;
    s.values.first = 5;
    s.values.second = 3.14;

    std::vector<uint8_t> bytes{};
    serialize<my_struct>(s, bytes);
    bytes.shrink_to_fit();
    std::cout << "Original struct size : " << sizeof(s) << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 8
  {
    struct my_struct {
      int value;

      struct nested_struct {
        float value;
      };
      nested_struct nested;
    };

    // construct
    my_struct s;
    s.value = 5;
    s.nested.value = 3.14f;

    // serialize
    auto bytes = serialize(s);

    // bytes: 0x0e 0x05 0x1b 0x15 0xc3 0xf5 0x48 0x40

    /*
      field:
        type: 0x0e (int32_as_int8)
        value: 0x05 (value = 5)

      field:
        type: 0x1b (struct)
        field:
          type: 0x15 (float32)
          value: 0xc3 0xf5 0x48 0x40 (value = 3.14f)
    */

    bytes.shrink_to_fit();
    std::cout << "Original struct size : " << sizeof(s) << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";
}
