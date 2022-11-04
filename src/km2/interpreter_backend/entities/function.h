#ifndef KM2_INTERPRETER_BACKEND_FUNCTION_H
#define KM2_INTERPRETER_BACKEND_FUNCTION_H

#include <string>
#include <list>
#include <memory>
#include "../../backend/entities/function.h"
#include <wall_e/src/utility/box.h>
#include "argument.h"

namespace km2 {
namespace interpreter_backend {

class type;
class block;
class dyn_function;
class native_function_provider;

class function : public km2::backend::function {
    friend block;
    std::string m_name;
    type* m_return_type;
    wall_e::box_list<argument> m_args;
    bool m_is_var_arg;

    wall_e::list<block*> m_blocks;

    const native_function_provider* m_native_function_provider;
    mutable std::shared_ptr<dyn_function> m_dyn;

    static wall_e::box_list<argument> born_args(const wall_e::vec<type*>& arg_types, function*);
public:
    function(
            const std::string& name,
            type* return_type,
            const wall_e::vec<type*>& arg_types,
            bool is_var_arg,
            const native_function_provider* native_function_provider
            )
        : m_name(name),
          m_return_type(return_type),
          m_args(born_args(arg_types, this)),
          m_is_var_arg(is_var_arg),
          m_native_function_provider(native_function_provider) {}

    std::shared_ptr<dyn_function> dyn() const;

    inline static std::unique_ptr<function> make_uniq(
            const std::string& name,
            type* return_type,
            const wall_e::vec<type*>& arg_types,
            bool is_var_arg,
            native_function_provider* native_function_provider
            ) { return std::make_unique<function>(name, return_type, arg_types, is_var_arg, native_function_provider); }

    inline static wall_e::box<function> make_box(
            const std::string& name,
            type* return_type,
            const wall_e::vec<type*>& arg_types,
            bool is_var_arg,
            native_function_provider* native_function_provider
            ) { return wall_e::box<function>::make(name, return_type, arg_types, is_var_arg, native_function_provider); }

    // value interface
public:
    virtual km2::backend::type *value_type() const override;

    // function interface
public:
    virtual bool is_var_arg() const override;
    virtual km2::backend::type *return_type() override;
    virtual std::string name() const override;
    virtual wall_e::vec<km2::backend::argument*> args() const override;
    virtual wall_e::list<km2::backend::block*> blocks() override;


    // printable interface
public:
    virtual std::ostream &print(std::ostream &) const override;
};


}
}
#endif // KM2_LLVM_BACKEND_FUNCTION_H
