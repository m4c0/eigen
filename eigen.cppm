export module eigen;
import hai;
import silog;

export namespace eigen {
class char_map_row {
  const char *m_begin;
  unsigned m_size;

public:
  constexpr char_map_row(const char *b, unsigned s) : m_begin{b}, m_size{s} {}

  [[nodiscard]] constexpr auto begin() const { return m_begin; }
  [[nodiscard]] constexpr auto end() const { return m_begin + m_size; }
};
class char_map_it {
  const char *m_begin;
  unsigned m_size;

public:
  constexpr char_map_it(const char *b, unsigned s) : m_begin{b}, m_size{s} {}

  [[nodiscard]] constexpr bool operator==(const char_map_it &o) const {
    return m_begin == o.m_begin;
  }
  [[nodiscard]] constexpr auto &operator++() {
    m_begin += m_size;
    return *this;
  }
  [[nodiscard]] constexpr auto operator*() const {
    return char_map_row{m_begin, m_size};
  }
};

class char_map {
  unsigned m_width;
  unsigned m_height;
  hai::array<char> m_data{m_width * m_height};

  [[nodiscard]] constexpr const char *row(unsigned y) const {
    return &m_data[y * m_width];
  }

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

  [[nodiscard]] constexpr auto begin() const {
    return char_map_it{row(0), m_width};
  }
  [[nodiscard]] constexpr auto end() const {
    return char_map_it{row(m_height), m_width};
  }

  inline void log(unsigned y_cols) const {
    for (auto y = 0; y < m_height; y++) {
      silog::log(silog::info, "%.*d: [%.*s]", y_cols, y + 1, m_width, row(y));
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
} // namespace eigen
