#include <cstdlib>
#include <cinttypes>
#include <cstring>

class N {
  public:
    char buffer[100];
    int32_t x;

    N(int32_t v) {
      x = v;
    }

    void setAnnotation(char *s) {
      memcpy(this->buffer, (void*)s, strlen(s));
    }

    uint32_t operator+(N v) {
      return this->x + v.x;
    }

    uint32_t operator-(N v) {
      return x - v.x;
    }
};

int main(int ac, char **av) {
  if (ac < 2) {
    exit(0);
  }
  N *x = new N(0x5);
  N *y = new N(0x6);

  (*x).setAnnotation(av[1]);
  return (*y + *x);
}
