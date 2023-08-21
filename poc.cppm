export module poc;
import silog;

static constexpr const auto w = 32;
static constexpr const auto h = 24;
static constexpr const auto ps = 3;

inline void log_row(char *row, unsigned y, unsigned yw, unsigned ys) {}

template <unsigned W, unsigned H> class char_map {
  char m_data[H][W];

public:
  constexpr char_map() {
    for (auto y = 0; y < H; y++) {
      for (auto x = 0; x < W; x++) {
        m_data[y][x] = ' ';
      }
    }
  }

  [[nodiscard]] constexpr auto &operator()(unsigned x, unsigned y) {
    silog::assert(x >= 0 && x < ps, "x out-of-bounds in char map modification");
    silog::assert(y >= 0 && y < ps, "y out-of-bounds in char map modification");
    return m_data[y][x];
  }

  inline void log(unsigned y_cols) const {
    for (auto y = 0; y < H; y++) {
      silog::log(silog::info, "%.*d: [%.*s]", y_cols, y + 1, W, m_data[y]);
    }
  }
};

class pattern {
  char_map<ps, ps> m_data{};

public:
  [[nodiscard]] constexpr auto &operator()(unsigned x, unsigned y) {
    return m_data(x, y);
  }

  inline void log() const { m_data.log(1); }
};

class map {
  char_map<w, h> m_data;

public:
  inline void log() const { m_data.log(2); }
};

extern "C" int main() {
  map m{};
  m.log();

  pattern p{};
  p(1, 1) = 'X';
  p.log();
}
