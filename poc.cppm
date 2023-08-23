export module poc;
import eigen;
import missingno;
import rng;
import silog;

using namespace eigen;

class map {
  static constexpr const auto w = 32;
  static constexpr const auto h = 24;

  char_map m_data{w, h};
  const pat_list *m_pats;

  [[nodiscard]] constexpr mno::opt<const pattern *> pat_at(unsigned x,
                                                           unsigned y) {
    auto p = m_data(x, y);
    if (p == ' ')
      return {};

    return mno::opt{&(*m_pats)[p - '0']};
  }
  [[nodiscard]] constexpr bool is_pat_valid(unsigned x, unsigned y,
                                            const pattern *p) {
    if (pat_at(x, y))
      return false;

    if (!pat_at(x + 1, y)
             .map([&](auto o) { return p->can_be_left_of(*o); })
             .unwrap(true))
      return false;

    if (!pat_at(x, y + 1)
             .map([&](auto o) { return p->can_be_top_of(*o); })
             .unwrap(true))
      return false;

    if (!pat_at(x - 1, y)
             .map([&](auto o) { return o->can_be_left_of(*p); })
             .unwrap(true))
      return false;

    if (!pat_at(x, y - 1)
             .map([&](auto o) { return o->can_be_top_of(*p); })
             .unwrap(true))
      return false;

    return true;
  }

public:
  explicit map(const pat_list *p) : m_pats{p} {}

  [[nodiscard]] constexpr auto begin() const { return m_data.begin(); }
  [[nodiscard]] constexpr auto end() const { return m_data.end(); }

  void fill_random_spot() {
    while (true) {
      auto x = rng::rand(w - 2) + 1;
      auto y = rng::rand(h - 2) + 1;
      auto p = rng::rand(m_pats->size());

      if (!is_pat_valid(x, y, &(*m_pats)[p]))
        continue;

      m_data(x, y) = p + '0';
      break;
    }
  }

  [[nodiscard]] auto expand() const {
    char_map exp{w * 3, h * 3};
    unsigned x = 0;
    unsigned y = 0;
    for (auto row : m_data) {
      for (auto col : row) {
        if (col != ' ') {
          auto &p = (*m_pats)[col - '0'];
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

  map m{&pats};
  for (auto i = 0; i < 400; i++) {
    m.fill_random_spot();
  }

  // m.log();
  m.expand().log(3);
}
