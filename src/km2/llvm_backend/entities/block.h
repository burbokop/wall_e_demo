#ifndef KM2_LLVM_BACKEND_BLOCK_H
#define KM2_LLVM_BACKEND_BLOCK_H

#include "../../backend/entities/block.h"
#include <memory>

namespace llvm {
class BasicBlock;
}

namespace km2 {
namespace llvm_backend {

class block : public backend::block {
    llvm::BasicBlock *m_data;
public:
    block(llvm::BasicBlock *data) : m_data(data) {}
    inline static std::unique_ptr<block> make_uniq(llvm::BasicBlock *data) { return std::make_unique<block>(data); }

    llvm::BasicBlock *data() const { return m_data; };
};

}
}

#endif // KM2_LLVM_BACKEND_BLOCK_H
