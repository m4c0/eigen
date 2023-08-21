export module poc;
import silog;

static constexpr const auto w = 32;
static constexpr const auto h = 24;
static constexpr const auto ps = 3;

inline void log_row(char *row, unsigned y, unsigned yw, unsigned ys) {
  silog::log(silog::info, "%.*d: [%.*s]", ys, y + 1, yw, row);
}

class pattern {
  char m_data[ps][ps]{};

public:
  pattern() {
    for (auto y = 0; y < ps; y++) {
      for (auto x = 0; x < ps; x++) {
        m_data[y][x] = ' ';
      }
    }
  }

  [[nodiscard]] constexpr auto &operator()(unsigned x, unsigned y) {
    silog::assert(x >= 0 && x < ps, "x out-of-bounds in pattern modification");
    silog::assert(y >= 0 && y < ps, "y out-of-bounds in pattern modification");
    return m_data[y][x];
  }

  inline void log() {
    for (auto y = 0; y < ps; y++) {
      log_row(m_data[y], y, ps, 1);
    }
  }
};

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

  inline void log() {
    for (auto y = 0; y < h; y++) {
      log_row(m_data[y], y, w, 3);
    }
  }
};

extern "C" int main() {
  map m{};
  m.log();

  pattern p{};
  p(1, 1) = 'X';
  p.log();
}
