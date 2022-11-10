#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>
#include <memory>
#include <limits>
#include <ostream>

#include <wall_e/src/utility/collections.h>

#include <wall_e/src/models/variant.h>
#include <wall_e/src/lex.h>
#include "size_t_ptr.h"

namespace km2 {
namespace interpreter_backend {

class execution_ctx;
class dyn_value;

class instruction {
    execution_ctx* m_ctx;
public:

    struct index {
        size_t_ptr offset = null_size_t_ptr;
        instruction* ptr = nullptr;
        void eval_ptr(execution_ctx* ctx);
        void force_eval_offset(execution_ctx* ctx);
        void eval_offset(execution_ctx* ctx);

        index next(execution_ctx* ctx) const;
        friend std::ostream& operator <<(std::ostream& stream, const index& i) {
            if(i.offset == null_size_t_ptr) {
                if(i.ptr) {
                    return stream << "{ offset: " << i.offset
                                  << ", ptr: " << i.ptr
                                  << " }";
                } else {
                    return stream << "null";
                }
            } else {
                return stream << i.offset;
            }
        }
    };

    struct exec_result {
        std::shared_ptr<dyn_value> value;
        index next_inst;
    };
    instruction(execution_ctx* ctx) : m_ctx(ctx) {}

    index next_index() const;

    inline execution_ctx* ctx() const { return m_ctx; };
    virtual exec_result exec(const wall_e::vec<std::shared_ptr<dyn_value>>&) = 0;
    virtual std::ostream& print(std::ostream&) const = 0;
    virtual void link() = 0;
};

}
}

#endif // INSTRUCTION_H
