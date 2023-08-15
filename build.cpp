#include "../ecow/ecow.hpp"

int main(int argc, char **argv) {
  using namespace ecow;

  auto all = unit::create<tool>("poc");
  all->add_unit<box>("poc");
  return run_main(all, argc, argv);
}
