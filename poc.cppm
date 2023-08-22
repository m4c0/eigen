export module poc;
import eigen;
import rng;
import silog;

extern "C" int putchar(char);

using namespace eigen;

class map {
  static constexpr const auto w = 32;
  static constexpr const auto h = 24;

  char_map m_data{w, h};
  const pat_list *m_pats;

public:
  explicit map(const pat_list *p) : m_pats{p} {}

  [[nodiscard]] constexpr auto begin() const { return m_data.begin(); }
  [[nodiscard]] constexpr auto end() const { return m_data.end(); }

  void fill_random_spot() {
    auto x = rng::rand(w);
    auto y = rng::rand(h);
    auto p = rng::rand(m_pats->size()) + '0';
    m_data(x, y) = p;
  }

  inline void log(bool raw = false) const {
    if (raw) {
      m_data.log(2);
      return;
    }
    for (auto row : m_data) {
      for (auto col : row) {
        putchar(col);
      }
      putchar('\n');
    }
  }
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
  rng::seed(69);

  map m{&pats};
  for (auto i = 0; i < 100; i++) {
    m.fill_random_spot();
  }

  m.log();
}
