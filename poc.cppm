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

  constexpr void set_row(unsigned y, const char (&s)[ps + 1]) {
    for (auto x = 0; x < ps; x++) {
      m_data(x, y) = s[x];
    }
  }

  inline void log() const {
    silog::log(silog::info, "probability: %f", m_prob);
    m_data.log(1);
  }
};
class pat_list {
  static constexpr const auto max_pats = 256;

  pattern m_pats[max_pats];
  unsigned m_count{};

public:
  [[nodiscard]] constexpr auto &operator[](unsigned p) {
    if (p >= m_count)
      m_count = p + 1;
    return m_pats[p];
  }
  [[nodiscard]] constexpr const auto &operator[](unsigned p) const {
    return m_pats[p];
  }

  [[nodiscard]] constexpr auto *begin() const noexcept { return &m_pats[0]; }
  [[nodiscard]] constexpr auto *end() const noexcept {
    return &m_pats[m_count];
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

  for (const auto &p : pats) {
    p.log();
  }
}
