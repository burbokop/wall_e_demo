#ifndef KM2_LLVM_BACKEND_FUNCTION_H
#define KM2_LLVM_BACKEND_FUNCTION_H

#include <string>
#include <list>
#include <memory>
#include "../../backend/entities/function.h"

namespace llvm {
class Function;
}

namespace km2 {
namespace llvm_backend {

class function : public backend::function {
    llvm::Function* m_data = nullptr;
public:
    function(llvm::Function* data) : m_data(data) {}
    inline static std::unique_ptr<llvm_backend::function> make_uniq(llvm::Function* data) { return std::make_unique<llvm_backend::function>(data); }

    llvm::Function* data() const { return m_data; };

    // value interface
public:
    virtual backend::type *value_type() const override;

    // function interface
public:
    virtual bool is_var_arg() const override;
    virtual wall_e::vec<backend::argument *> args() override;
    virtual backend::type *return_type() override;
    virtual std::string name() override;
};

}
}
#endif // KM2_LLVM_BACKEND_FUNCTION_H
