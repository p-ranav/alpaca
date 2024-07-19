#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

constexpr auto OPTIONS = options::fixed_length_encoding | options::force_aligned_access;

TEST_CASE("Fixed length encoding") {

  struct Simple {
    uint32_t field{0};
  };

  Simple data{0x12345678};

  std::vector<uint8_t> bytes;
  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed = alpaca::deserialize<OPTIONS, Simple>(bytes, err);

  REQUIRE(!err);

  REQUIRE(reconstructed.field == data.field); // And here it dies.
}

TEST_CASE("Nested Structs") {
  struct Inner {
    uint32_t field1{0};
    std::string field2;
  };

  struct Outer {
    Inner inner;
    uint32_t field3{0};
  };

  Outer data{{0x12345678, "test"}, 0x87654321};

  std::vector<uint8_t> bytes;

  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed = alpaca::deserialize<OPTIONS, Outer>(bytes, err);

  REQUIRE(!err);
  REQUIRE(reconstructed.inner.field1 == data.inner.field1);
  REQUIRE(reconstructed.inner.field2 == data.inner.field2);
  REQUIRE(reconstructed.field3 == data.field3);
}

TEST_CASE("Vector of Custom Structs") {
  struct Item {
    uint32_t id{0};
    std::string name;
  };

  struct Container {
    std::vector<Item> items;
  };

  Container data{{{1, "item1"}, {2, "item2"}, {3, "item3"}}};

  std::vector<uint8_t> bytes;

  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed = alpaca::deserialize<OPTIONS, Container>(bytes, err);
  REQUIRE(!err);

  REQUIRE(reconstructed.items.size() == data.items.size());
  for (size_t i = 0; i < data.items.size(); ++i) {
    REQUIRE(reconstructed.items[i].id == data.items[i].id);
    REQUIRE(reconstructed.items[i].name == data.items[i].name);
  }
}

TEST_CASE("Struct with String") {
  struct SimpleWithString {
    uint32_t id{0};
    std::string name;
  };

  SimpleWithString data{0x12345678, "test"};

  std::vector<uint8_t> bytes;

  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed =
    alpaca::deserialize<OPTIONS, SimpleWithString>(bytes, err);

  REQUIRE(!err);
  REQUIRE(reconstructed.id == data.id);
  REQUIRE(reconstructed.name == data.name);
}

// Aggiungi qui altri casi di test...
TEST_CASE("Complex Nested Structs and Vectors") {
  struct Inner {
    uint32_t id{0};
    std::string name;
  };

  struct Middle {
    Inner inner;
    std::vector<Inner> innerList;
  };

  struct Outer {
    Middle middle;
    std::string description;
  };

  Outer data{{{0x1, "inner1"}, {{0x2, "inner2"}, {0x3, "inner3"}}},
             "complex structure"};

  std::vector<uint8_t> bytes;

  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed = alpaca::deserialize<OPTIONS, Outer>(bytes, err);

  REQUIRE(!err);
  REQUIRE(reconstructed.middle.inner.id == data.middle.inner.id);
  REQUIRE(reconstructed.middle.inner.name == data.middle.inner.name);
  REQUIRE(reconstructed.middle.innerList.size() ==
          data.middle.innerList.size());
  for (size_t i = 0; i < data.middle.innerList.size(); ++i) {
    REQUIRE(reconstructed.middle.innerList[i].id ==
            data.middle.innerList[i].id);
    REQUIRE(reconstructed.middle.innerList[i].name ==
            data.middle.innerList[i].name);
  }
  REQUIRE(reconstructed.description == data.description);
}

TEST_CASE("Backwards Compatibility (new field added in struct)") {
  struct Simple {
    uint32_t field{0};
  };

  Simple data{0x12345678};

  std::vector<uint8_t> bytes;

  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed = alpaca::deserialize<OPTIONS, Simple>(bytes, err);

  REQUIRE(!err);
  REQUIRE(reconstructed.field == data.field);
}

// Altri casi di test...

TEST_CASE("long strings") {
  struct WithLongString {
    std::string long_string;
  };

  WithLongString data{"Questo è un test con una stringa molto lunga. " +
                      std::string(1000, 'A')};

  std::vector<uint8_t> bytes;

  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed =
    alpaca::deserialize<OPTIONS, WithLongString>(bytes, err);

  REQUIRE(!err);
  REQUIRE(reconstructed.long_string == data.long_string);
}

TEST_CASE("empty strings") {
  struct WithNullString {
    std::string null_string;
  };

  WithNullString data{""};

  std::vector<uint8_t> bytes;

  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed =
    alpaca::deserialize<OPTIONS, WithNullString>(bytes, err);

  REQUIRE(!err);
  REQUIRE(reconstructed.null_string == data.null_string);
}

TEST_CASE("32-bit extremes") {
  struct WithExtremeInt32 {
    int32_t min_int{std::numeric_limits<int32_t>::min()};
    int32_t max_int{std::numeric_limits<int32_t>::max()};
  };

  WithExtremeInt32 data;

  std::vector<uint8_t> bytes;

  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed =
    alpaca::deserialize<OPTIONS, WithExtremeInt32>(bytes, err);

  REQUIRE(!err);
  REQUIRE(reconstructed.min_int == data.min_int);
  REQUIRE(reconstructed.max_int == data.max_int);
}

TEST_CASE("64-bit extreme") {
  struct WithExtremeInt64 {
    int64_t min_int{std::numeric_limits<int64_t>::min()};
    int64_t max_int{std::numeric_limits<int64_t>::max()};
  };

  WithExtremeInt64 data;

  std::vector<uint8_t> bytes;

  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed =
    alpaca::deserialize<OPTIONS, WithExtremeInt64>(bytes, err);

  REQUIRE(!err);
  REQUIRE(reconstructed.min_int == data.min_int);
  REQUIRE(reconstructed.max_int == data.max_int);
}

TEST_CASE("Nested Structs with Extreme Values") {
  struct Inner {
    int32_t min_int{std::numeric_limits<int32_t>::min()};
    int32_t max_int{std::numeric_limits<int32_t>::max()};
    std::string long_string;
  };

  struct Outer {
    Inner inner;
    int64_t extreme_value{std::numeric_limits<int64_t>::max()};
  };

  Outer data{{std::numeric_limits<int32_t>::max(),
                std::numeric_limits<int32_t>::min(), std::string(1000, 'A')},
             std::numeric_limits<int64_t>::max()};

  std::vector<uint8_t> bytes;

  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed = alpaca::deserialize<OPTIONS, Outer>(bytes, err);

  REQUIRE(!err);
  REQUIRE(reconstructed.inner.min_int == data.inner.min_int);
  REQUIRE(reconstructed.inner.max_int == data.inner.max_int);
  REQUIRE(reconstructed.inner.long_string == data.inner.long_string);
  REQUIRE(reconstructed.extreme_value == data.extreme_value);
}

TEST_CASE("Complex Nested Structs and Vectors with Extremes") {
  struct Inner {
    int32_t min_int{std::numeric_limits<int32_t>::min()};
    int32_t max_int{std::numeric_limits<int32_t>::max()};
    std::string long_string;
  };

  struct Middle {
    Inner inner;
    std::vector<Inner> innerList;
  };

  struct Outer {
    Middle middle;
    std::string description;
  };
  constexpr auto min = std::numeric_limits<int32_t>::min();
  constexpr auto max = std::numeric_limits<int32_t>::max();

  Outer data{{{min, max, std::string(1000, 'A')},
              {{min, max, std::string(500, 'B')},
               {min, max, std::string(500, 'C')}}},
             "complex structure with extremes"};

  std::vector<uint8_t> bytes;

  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed = alpaca::deserialize<OPTIONS, Outer>(bytes, err);

  REQUIRE(!err);
  REQUIRE(reconstructed.middle.inner.min_int == data.middle.inner.min_int);
  REQUIRE(reconstructed.middle.inner.max_int == data.middle.inner.max_int);
  REQUIRE(reconstructed.middle.inner.long_string ==
          data.middle.inner.long_string);
  REQUIRE(reconstructed.middle.innerList.size() ==
          data.middle.innerList.size());
  for (size_t i = 0; i < data.middle.innerList.size(); ++i) {
    REQUIRE(reconstructed.middle.innerList[i].min_int ==
            data.middle.innerList[i].min_int);
    REQUIRE(reconstructed.middle.innerList[i].max_int ==
            data.middle.innerList[i].max_int);
    REQUIRE(reconstructed.middle.innerList[i].long_string ==
            data.middle.innerList[i].long_string);
  }
  REQUIRE(reconstructed.description == data.description);
}

TEST_CASE("Complex Struct with Many Fields and Vector of Detailed Structs") {

  struct Detail {
    int32_t id{0};
    int64_t value1{0};
    int64_t value2{0};
    double ratio{0.0};
    std::string description;
    uint32_t count{0};
    bool flag{false};
    std::string note;
    float balance{0.0f};
    char initial{'A'};
    uint64_t big_value{0};
    int16_t short_value{0};
    enum class Status { ACTIVE, INACTIVE, PENDING, DELETED };
    Status status{Status::ACTIVE};
    std::vector<int> numbers;
    std::string metadata;
  };

  struct Complex {
    int32_t field1{0};
    int64_t field2{0};
    std::string field3;
    double field4{0.0};
    Detail::Status status{Detail::Status::ACTIVE};
    std::vector<Detail> details;
    bool flag{false};
    uint32_t field5{0};
    float field6{0.0f};
    std::string field7;
    char field8{'A'};
    uint64_t field9{0};
    int16_t field10{0};
  };

  Complex data{
    0x12345678,              // field1
    0x123456789ABCDEF0,      // field2
    "This is a test string", // field3
    1234.5678,               // field4
    Detail::Status::PENDING,         // status
    {                        // details
      {1,
       100,
       200,
       0.1,
       "Detail 1",
       10,
       true,
       "Note 1",
       10.5f,
       'B',
       0xFFFFFFFF,
       1000,
       Detail::Status::ACTIVE,
       {1, 2, 3},
       "Meta 1"},
      {2,
       200,
       400,
       0.2,
       "Detail 2",
       20,
       false,
       "Note 2",
       20.5f,
       'C',
       0xFFFFFFFE,
       2000,
       Detail::Status::INACTIVE,
       {4, 5, 6},
       "Meta 2"},
      {3,
       300,
       600,
       0.3,
       "Detail 3",
       30,
       true,
       "Note 3",
       30.5f,
       'D',
       0xFFFFFFFD,
       3000,
       Detail::Status::DELETED,
       {7, 8, 9},
       "Meta 3"}},
    true,                  // flag
    0x87654321,            // field5
    5678.1234f,            // field6
    "Another test string", // field7
    'Z',                   // field8
    0xFEDCBA9876543210,    // field9
    -32768                 // field10
  };

  std::vector<uint8_t> bytes;
  alpaca::serialize<OPTIONS>(data, bytes);

  std::error_code err;
  auto reconstructed = alpaca::deserialize<OPTIONS, Complex>(bytes, err);

  REQUIRE(!err);
  REQUIRE(reconstructed.field1 == data.field1);
  REQUIRE(reconstructed.field2 == data.field2);
  REQUIRE(reconstructed.field3 == data.field3);
  REQUIRE(reconstructed.field4 == data.field4);
  REQUIRE(reconstructed.status == data.status);
  REQUIRE(reconstructed.flag == data.flag);
  REQUIRE(reconstructed.field5 == data.field5);
  REQUIRE(reconstructed.field6 == data.field6);
  REQUIRE(reconstructed.field7 == data.field7);
  REQUIRE(reconstructed.field8 == data.field8);
  REQUIRE(reconstructed.field9 == data.field9);
  REQUIRE(reconstructed.field10 == data.field10);

  REQUIRE(reconstructed.details.size() == data.details.size());
  for (size_t i = 0; i < data.details.size(); ++i) {
    REQUIRE(reconstructed.details[i].id == data.details[i].id);
    REQUIRE(reconstructed.details[i].value1 == data.details[i].value1);
    REQUIRE(reconstructed.details[i].value2 == data.details[i].value2);
    REQUIRE(reconstructed.details[i].ratio == data.details[i].ratio);
    REQUIRE(reconstructed.details[i].description ==
            data.details[i].description);
    REQUIRE(reconstructed.details[i].count == data.details[i].count);
    REQUIRE(reconstructed.details[i].flag == data.details[i].flag);
    REQUIRE(reconstructed.details[i].note == data.details[i].note);
    REQUIRE(reconstructed.details[i].balance == data.details[i].balance);
    REQUIRE(reconstructed.details[i].initial == data.details[i].initial);
    REQUIRE(reconstructed.details[i].big_value == data.details[i].big_value);
    REQUIRE(reconstructed.details[i].short_value ==
            data.details[i].short_value);
    REQUIRE(reconstructed.details[i].status == data.details[i].status);
    REQUIRE(reconstructed.details[i].numbers == data.details[i].numbers);
    REQUIRE(reconstructed.details[i].metadata == data.details[i].metadata);
  }
}
