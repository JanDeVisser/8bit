#include "backplane.h"
#include "iochannel.h"

struct Terminal {
  byte       value = 0;
  bool       flag = false;
  IOChannel *channel;

  Terminal() {
    channel = new IOChannel(0x01, "Terminal", [this](byte val) {
      this->value = val;
      flag = true;
    });
    channel -> setLowClockHandler([this]() {
      if (flag) {
        std::cout << value;
        flag = false;
      }
    });
  }

  virtual ~Terminal() {
    delete channel;
  }
};

Terminal * makeTerminal(BackPlane *bp) {
  Terminal *ret = new Terminal();
  bp -> insertIO(ret -> channel);
}

int main(int argc, char **argv) {
  BackPlane *bp = new BackPlane();
  bp -> defaultSetup();
  terminal = makeTerminal(bp);

  delete Terminal;
}