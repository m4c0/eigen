export module poc;
import silog;

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
    return m_data[y][x];
  }
  [[nodiscard]] constexpr auto operator()(unsigned x, unsigned y) const {
    return m_data[y][x];
  }

  inline void log(unsigned y_cols) const {
    for (auto y = 0; y < H; y++) {
      silog::log(silog::info, "%.*d: [%.*s]", y_cols, y + 1, W, m_data[y]);
    }
  }
};

static constexpr const auto w = 32;
static constexpr const auto h = 24;
static constexpr const auto ps = 3;

class pattern {
  char_map<ps, ps> m_data{};

public:
  [[nodiscard]] constexpr auto &operator()(unsigned x, unsigned y) {
    return m_data(x, y);
  }

  constexpr void set_row(unsigned y, const char (&s)[4]) {
    m_data(0, y) = s[0];
    m_data(1, y) = s[1];
    m_data(2, y) = s[2];
  }

  inline void log() const { m_data.log(1); }
};

class map {
  char_map<w, h> m_data;

public:
  inline void log() const { m_data.log(2); }
};

constexpr auto p0 = [] {
  pattern p{};
  p.set_row(0, "   ");
  p.set_row(1, " ..");
  p.set_row(2, " ..");
  return p;
}();

extern "C" int main() {
  map m{};
  m.log();

  p0.log();
}
