export module poc;
import silog;

static constexpr const auto w = 32;
static constexpr const auto h = 24;

class map {
  char m_data[h][w]{};

public:
  map() {
    for (auto y = 0; y < h; y++) {
      for (auto x = 0; x < w; x++) {
        m_data[y][x] = ' ';
      }
    }
  }

  void print() {
    for (auto y = 0; y < h; y++) {
      silog::log(silog::info, "%3d: [%.*s]", y + 1, w, m_data[y]);
    }
  }
};

extern "C" int main() {
  map m{};
  m.print();
}
