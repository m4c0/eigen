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
  float m_prob{};

public:
  [[nodiscard]] constexpr auto &operator()(unsigned x, unsigned y) {
    return m_data(x, y);
  }
  [[nodiscard]] constexpr auto &probability() noexcept { return m_prob; }

  constexpr void set_row(unsigned y, const char (&s)[4]) {
    m_data(0, y) = s[0];
    m_data(1, y) = s[1];
    m_data(2, y) = s[2];
  }

  inline void log() const {
    silog::log(silog::info, "probability: %f", m_prob);
    m_data.log(1);
  }
};
class pat_list {
  static constexpr const auto max_pats = 256;

  pattern m_pats[max_pats];

public:
  [[nodiscard]] constexpr auto &operator[](unsigned p) { return m_pats[p]; }
  [[nodiscard]] constexpr const auto &operator[](unsigned p) const {
    return m_pats[p];
  }
};

class map {
  char_map<w, h> m_data;

public:
  inline void log() const { m_data.log(2); }
};

constexpr const auto pats = [] {
  pat_list p{};

  p[0].set_row(0, "   ");
  p[0].set_row(1, " ..");
  p[0].set_row(2, " ..");
  p[0].probability() = 1.f;

  p[1].set_row(0, " ..");
  p[1].set_row(1, " ..");
  p[1].set_row(2, " ..");
  p[1].probability() = 1.f;

  p[2].set_row(0, "...");
  p[2].set_row(1, "...");
  p[2].set_row(2, "...");
  p[2].probability() = 1.f;

  p[3].set_row(0, "   ");
  p[3].set_row(1, "   ");
  p[3].set_row(2, "   ");
  p[3].probability() = 1.f;

  return p;
}();

extern "C" int main() {
  map m{};
  m.log();

  pats[0].log();
  pats[1].log();
  pats[2].log();
  pats[3].log();
}
