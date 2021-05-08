
#include <memory>

namespace video {

struct Demux {
  virtual ~Demux() {}
};


std::unique_ptr<Demux> CreateDemux();

} // namespace video
