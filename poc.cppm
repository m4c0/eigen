export module poc;
import eigen;
import missingno;
import rng;
import silog;

using namespace eigen;

class map {
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
    if (p == ' ')
      return {};

    return mno::opt{&(*m_pats)[p - 'A']};
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

        m_data(px, py) = ' ';
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
      if (is_pat_valid(3, x, y, &(*m_pats)[p])) {
        m_data(x, y) = p + 'A';
        return true;
      }
      p = (p + 1) % m_pats->size();
    }
    return false;
  }

  void set_random_spot(unsigned p) {
    auto x = rng::rand(w() - 2) + 1;
    auto y = rng::rand(h() - 2) + 1;
    m_data(x, y) = p + 'A';
  }

  void fill_random_spot() {
    auto x = rng::rand(w());
    auto y = rng::rand(h());
    for (auto c = 0; c < w() * h(); c++) {
      if (m_data(x, y) == ' ') {
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

  [[nodiscard]] auto expand() const {
    char_map exp{w() * 3, h() * 3};
    unsigned x = 0;
    unsigned y = 0;
    for (auto row : m_data) {
      for (auto col : row) {
        if (col != ' ') {
          auto &p = (*m_pats)[col - 'A'];
          for (auto dy = 0; dy < 3; dy++) {
            for (auto dx = 0; dx < 3; dx++) {
              exp(x + dx, y + dy) = p(dx, dy);
            }
          }
        }
        x += 3;
      }
      x = 0;
      y += 3;
    }
    return exp;
  }

  inline void log() const { m_data.log(2); }
};

static constexpr const auto w = 32 * 2;
static constexpr const auto h = 24 * 2;

constexpr const auto pats = [] {
  pat_list p{};

  p[0].set_row(0, "...");
  p[0].set_row(1, ".XX");
  p[0].set_row(2, ".XX");
  p[0].probability() = 1.f;

  p[1].set_row(0, ".XX");
  p[1].set_row(1, ".XX");
  p[1].set_row(2, ".XX");
  p[1].probability() = 1.f;

  p[2].set_row(0, "XXX");
  p[2].set_row(1, "XXX");
  p[2].set_row(2, "XXX");
  p[2].probability() = 1.f;

  p[3].set_row(0, "...");
  p[3].set_row(1, "...");
  p[3].set_row(2, "...");
  p[3].probability() = 1.f;

  (p[4] = p[0]).rotate();
  (p[5] = p[4]).rotate();
  (p[6] = p[5]).rotate();

  (p[7] = p[1]).rotate();
  (p[8] = p[7]).rotate();
  (p[9] = p[8]).rotate();

  p[10].set_row(0, "XXX");
  p[10].set_row(1, "XXX");
  p[10].set_row(2, ".XX");

  (p[11] = p[10]).rotate();
  (p[12] = p[11]).rotate();
  (p[13] = p[12]).rotate();

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
  rng::seed(69);

  silog::log(silog::info, "starting");

  map m{&pats, w, h};
  for (auto i = 0; i < w; i++) {
    m(i, 0) = 2 + 'A';
    m(i, h - 1) = 2 + 'A';
  }
  for (auto i = 0; i < h; i++) {
    m(0, i) = 2 + 'A';
    m(w - 1, i) = 2 + 'A';
  }
  for (auto i = 0; i < w * 4; i++) {
    m.set_random_spot(2);
  }
  for (auto i = 0; i < w * h; i++) {
    m.fill_random_spot();
  }

  m.log();
  // m.expand().log(3);
}
