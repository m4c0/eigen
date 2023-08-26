export module poc;
import eigen;
import rng;
import silog;

using namespace eigen;

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

void dump(const map &m) {
  constexpr const auto y_cols = 2;

  char buf[1024];
  for (char &c : buf)
    c = ' ';
  for (auto x = 1; x <= w / 10; x++) {
    buf[x * 10] = '0' + x;
  }
  silog::log(silog::info, "%*s  .%.*s.", y_cols, "", w, buf);
  for (char &c : buf)
    c = ' ';
  for (auto x = 0; x <= w; x++) {
    buf[x] = '0' + x % 10;
  }
  silog::log(silog::info, "%*s  .%.*s.", y_cols, "", w, buf);

  unsigned y = 0;
  for (auto row : m) {
    silog::log(silog::info, "%.*d: [%.*s]", y_cols, y++, w, row.begin());
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

  dump(m);
  // m.expand().log(3);
}
