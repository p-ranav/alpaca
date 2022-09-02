#include <alpaca/alpaca.h>

int main() {

  struct Config {
    std::string device;
    std::pair<unsigned, unsigned> resolution;
    std::array<double, 9> K_matrix;
    std::vector<float> distortion_coeffients;
    std::map<std::string, std::variant<uint16_t, std::string, bool>> parameters;
  };

  // Construct the object
  Config c{"/dev/video0",
           {640, 480},
           {223.28249888247538, 0.0, 152.30570853111396, 0.0, 223.8756535707556,
            124.5606000035353, 0.0, 0.0, 1.0},
           {-0.44158343539568284, 0.23861463831967872, 0.0016338407443826572,
            0.0034950038632981604, -0.05239245892096022},
           {{"start_server", bool{true}},
            {"max_depth", uint16_t{5}},
            {"model_path", std::string{"foo/bar.pt"}}}};

  // Serialize
  std::vector<uint8_t> bytes;
  auto bytes_written = alpaca::serialize(c, bytes);

  // Deserialize
  std::error_code ec;
  auto recovered = alpaca::deserialize<Config>(bytes, ec);
  assert((bool)ec == false);
  assert(recovered.device == std::string{"/dev/video0"});
  assert((recovered.resolution == std::pair<unsigned, unsigned>{640, 480}));
  assert((recovered.K_matrix ==
          std::array<double, 9>{223.28249888247538, 0.0, 152.30570853111396,
                                0.0, 223.8756535707556, 124.5606000035353, 0.0,
                                0.0, 1.0}));
  assert((recovered.distortion_coeffients ==
          std::vector<float>{-0.44158343539568284, 0.23861463831967872,
                             0.0016338407443826572, 0.0034950038632981604,
                             -0.05239245892096022}));
  assert((recovered.parameters ==
          std::map<std::string, std::variant<uint16_t, std::string, bool>>{
              {"start_server", bool{true}},
              {"max_depth", uint16_t{5}},
              {"model_path", std::string{"foo/bar.pt"}}}));
}