#ifndef KM2_INTERPRETER_BACKEND_BLOCK_H
#define KM2_INTERPRETER_BACKEND_BLOCK_H

#include "../../backend/entities/block.h"
#include <memory>
#include <wall_e/src/utility/collections.h>
#include <wall_e/src/utility/box.h>

namespace km2 {
namespace interpreter_backend {

class execution_ctx;
class instruction;
class function;

class block : public km2::backend::block {
    std::string m_name;
    function* m_f;
    execution_ctx* m_ctx;

    wall_e::vec<instruction*> m_instructions;
public:
    block(execution_ctx* ctx, const std::string& name, function* f);

    void append_instruction(instruction* i) { m_instructions.push_back(i); }
    void append_instructions(const wall_e::vec<instruction*>& i) { m_instructions.append(i); }
    const wall_e::vec<instruction*>& instructions() const { return m_instructions; };

    inline static km2::backend::block* upcast(block* t) { return dynamic_cast<km2::backend::block*>(t); }

    inline static std::unique_ptr<block> make_uniq(
            execution_ctx* ctx,
            const std::string& name,
            function* f
        ) { return std::make_unique<block>(ctx, name, f); }

    inline static wall_e::box<block> make_box(
            execution_ctx* ctx,
            const std::string& name,
            function* f
        ) { return wall_e::box<block>::make(ctx, name, f); }

    // block interface
public:
    virtual std::string name() const override;

    // printable interface
public:
    virtual std::ostream &print(std::ostream &) const override;
};

}
}

#endif // KM2_LLVM_BACKEND_BLOCK_H
