export module poc;
import hai;
import silog;

class char_map {
  unsigned m_width;
  unsigned m_height;
  hai::varray<char> m_data{m_width * m_height};

public:
  constexpr char_map(unsigned w, unsigned h) : m_width{w}, m_height{h} {
    for (auto &c : m_data) {
      c = ' ';
    }
  }

  [[nodiscard]] constexpr auto &operator()(unsigned x, unsigned y) {
    return m_data[y * m_width + x];
  }
  [[nodiscard]] constexpr auto operator()(unsigned x, unsigned y) const {
    return m_data[y * m_width + x];
  }

  inline void log(unsigned y_cols) const {
    for (auto y = 0; y < m_height; y++) {
      silog::log(silog::info, "%.*d: [%.*s]", y_cols, y + 1, m_width,
                 &m_data[y * m_width]);
    }
  }
};

class pattern {
  static constexpr const auto ps = 3;

  char m_data[ps][ps]{};
  float m_prob{};

public:
  [[nodiscard]] constexpr auto &operator()(unsigned x, unsigned y) {
    return m_data[y][x];
  }
  [[nodiscard]] constexpr auto operator()(unsigned x, unsigned y) const {
    return m_data[y][x];
  }
  [[nodiscard]] constexpr auto &probability() noexcept { return m_prob; }

  constexpr void set_row(unsigned y, const char (&s)[ps + 1]) {
    for (auto x = 0; x < ps; x++) {
      (*this)(x, y) = s[x];
    }
  }

  constexpr void rotate() noexcept {
    static_assert(ps == 3);

    auto t = (*this)(0, 0);
    (*this)(0, 0) = (*this)(2, 0);
    (*this)(2, 0) = (*this)(2, 2);
    (*this)(2, 2) = (*this)(0, 2);
    (*this)(0, 2) = t;

    t = (*this)(1, 0);
    (*this)(1, 0) = (*this)(2, 1);
    (*this)(2, 1) = (*this)(1, 2);
    (*this)(1, 2) = (*this)(0, 1);
    (*this)(0, 1) = t;
  }

  [[nodiscard]] constexpr bool can_be_left_of(const pattern &o) const noexcept {
    for (auto i = 0; i < ps; i++) {
      if ((*this)(ps - 1, i) != o(0, i))
        return false;
    }
    return true;
  }
  [[nodiscard]] constexpr bool can_be_top_of(const pattern &o) const noexcept {
    for (auto i = 0; i < ps; i++) {
      if ((*this)(i, ps - 1) != o(i, 0))
        return false;
    }
    return true;
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

  [[nodiscard]] constexpr auto size() const noexcept { return m_count; }
  [[nodiscard]] constexpr auto *begin() const noexcept { return &m_pats[0]; }
  [[nodiscard]] constexpr auto *end() const noexcept {
    return &m_pats[m_count];
  }
};

class map {
  static constexpr const auto w = 32;
  static constexpr const auto h = 24;

  char_map m_data{w, h};

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

  (p[4] = p[0]).rotate();
  (p[5] = p[4]).rotate();
  (p[6] = p[5]).rotate();

  (p[7] = p[1]).rotate();
  (p[8] = p[7]).rotate();
  (p[9] = p[8]).rotate();

  return p;
}();

void dump_possible_connections() {
  for (auto a = 0; a < pats.size(); a++) {
    for (auto b = 0; b < pats.size(); b++) {
      bool l = (pats[a].can_be_left_of(pats[b]));
      bool y = (pats[a].can_be_top_of(pats[b]));
      if (l || y)
        silog::log(silog::debug, "%d can be %s %s of %d", a, (l ? "left" : ""),
                   (y ? "top" : ""), b);
    }
  }
}

extern "C" int main() {
  map m{};
  m.log();
}
