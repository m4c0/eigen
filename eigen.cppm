export module eigen;
import hai;
import missingno;
import rng;
import silog;

export namespace eigen {
inline constexpr const char nil = 0xFF;

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
      c = nil;
    }
  }

  [[nodiscard]] constexpr auto width() const noexcept { return m_width; }
  [[nodiscard]] constexpr auto height() const noexcept { return m_height; }

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
  static constexpr const auto recursiveness = 3;

  char_map m_data;
  const pat_list *m_pats;

  [[nodiscard]] constexpr auto w() const noexcept { return m_data.width(); }
  [[nodiscard]] constexpr auto h() const noexcept { return m_data.height(); }

  [[nodiscard]] constexpr auto oob(unsigned x, unsigned y) const noexcept {
    return (x < 0 || x >= w() || y < 0 || y >= h());
  }

  [[nodiscard]] constexpr mno::opt<const pattern *> pat_at(unsigned x,
                                                           unsigned y) {
    auto p = m_data(x, y);
    if (p == nil)
      return {};

    return mno::opt{&(*m_pats)[p]};
  }
  [[nodiscard]] constexpr bool is_valid(unsigned max_l, unsigned x, unsigned y,
                                        auto &&fn) {
    if (oob(x, y))
      return false;

    return pat_at(x, y).map(fn).unwrap([&] -> bool {
      for (const auto &p : *m_pats) {
        if (!fn(&p))
          continue;

        if (is_pat_valid(max_l - 1, x, y, &p))
          return true;
      }
      return false;
    });
  }

  [[nodiscard]] constexpr bool is_pat_valid(unsigned max_l, unsigned x,
                                            unsigned y, const pattern *p) {
    if (oob(x, y))
      return false;
    if (max_l == 0)
      return true;

    if (pat_at(x, y))
      return false;

    if (!is_valid(max_l, x + 1, y,
                  [&](auto *o) { return p->can_be_left_of(*o); }))
      return false;

    if (!is_valid(max_l, x, y + 1,
                  [&](auto *o) { return p->can_be_top_of(*o); }))
      return false;

    if (!is_valid(max_l, x - 1, y,
                  [&](auto *o) { return o->can_be_left_of(*p); }))
      return false;

    if (!is_valid(max_l, x, y - 1,
                  [&](auto *o) { return o->can_be_top_of(*p); }))
      return false;

    return true;
  }

  void explode(unsigned cx, unsigned cy) {
    for (auto y = -1; y <= 1; y++) {
      auto py = y + cy;
      if (py < 0 || py >= h())
        continue;

      for (auto x = -1; x <= 1; x++) {
        auto px = x + cx;
        if (px < 0 || px >= w())
          continue;

        m_data(px, py) = nil;
      }
    }
  }

public:
  explicit map(const pat_list *p, unsigned w, unsigned h)
      : m_pats{p}, m_data{w, h} {}

  [[nodiscard]] constexpr auto begin() const { return m_data.begin(); }
  [[nodiscard]] constexpr auto end() const { return m_data.end(); }

  [[nodiscard]] constexpr auto &operator()(unsigned x, unsigned y) {
    return m_data(x, y);
  }

  [[nodiscard]] bool fill_at(unsigned x, unsigned y) {
    auto p = rng::rand(m_pats->size());

    for (auto _ : *m_pats) {
      if (is_pat_valid(recursiveness, x, y, &(*m_pats)[p])) {
        m_data(x, y) = p;
        return true;
      }
      p = (p + 1) % m_pats->size();
    }
    return false;
  }

  void set_random_spot(unsigned p) {
    auto x = rng::rand(w() - 2) + 1;
    auto y = rng::rand(h() - 2) + 1;
    m_data(x, y) = p;
  }

  void fill_random_spot() {
    auto x = rng::rand(w());
    auto y = rng::rand(h());
    for (auto c = 0; c < w() * h(); c++) {
      if (m_data(x, y) == nil) {
        if (fill_at(x, y))
          return;

        explode(x, y);
        // silog::log(silog::debug, "conflict at %d %d", x, y);
      }

      x++;
      if (x >= w()) {
        x = x % w();
        y = (y + 1) % h();
      }
    }
  }

  void fill() {
    for (auto i = 0; i < w() * h(); i++) {
      fill_random_spot();
    }
  }
  void draw_border(unsigned p) {
    for (auto i = 0; i < w(); i++) {
      m_data(i, 0) = p;
      m_data(i, h() - 1) = p;
    }
    for (auto i = 0; i < h(); i++) {
      m_data(0, i) = p;
      m_data(w() - 1, i) = p;
    }
  }
};
} // namespace eigen
