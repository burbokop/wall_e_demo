#ifndef KM2_BACKEND_UNIT_H
#define KM2_BACKEND_UNIT_H

#include "capabilities/abstract_capability.h"
#include "wall_e/src/utility/typename.h"
#include <map>
#include <typeinfo>
#include <memory>
#include <wall_e/src/utility/collections.h>
#include <wall_e/src/models/either.h>
#include <cassert>

namespace km2 {
namespace backend {

class value;
class type;
class block;
class function;
class backend;
class argument;

template<typename T>
class cap_factory {
    virtual T* new_cap() = 0;
};

class unit {
    km2_backend_decl_as
    mutable std::map<std::size_t, abstract_capability* /* deleting with unit */> m_capabilities;
    const backend* m_backend;

    abstract_capability* new_capability(std::size_t hash) const;
public:
    typedef std::function<std::shared_ptr<unit>(const backend*)> factory;

    enum ArgSettingStatus {
        ArgSettingSuccess,
        ArgSettingDublicates,
        ArgSettingEmptyStack
    };
    unit(const backend* b) : m_backend(b) {}
    virtual ~unit();


    template<typename T>
    T* cap() const {
        static_assert (std::is_base_of<abstract_capability, T>::value, "T must be inherited from km2::abstract_capability");
        const auto& hash = typeid (T).hash_code();
        const auto it = m_capabilities.find(hash);
        if(it == m_capabilities.end()) {
            const auto& result = dynamic_cast<T*>(new_capability(typeid(T).hash_code()));
            if(!result) {
                throw std::runtime_error("can not create capapbility of type: " + wall_e::type_name<T>() + " (hash: " + std::to_string(typeid(T).hash_code()) + "). factory not found");
            }
            m_capabilities.insert(std::pair<std::size_t, abstract_capability*> { hash, result });
            assert(result->unit());
            return result;
        } else {
            return dynamic_cast<T*>(it->second);
        }
    }

    // TODO std::shared_ptr<value> execute(const std::function<std::shared_ptr<value>(llvm::LLVMContext*, llvm::IRBuilder<>*, llvm::Module*)>& func);
    virtual ArgSettingStatus set_arg(const std::string& name, value* val) = 0;
    virtual value* arg(const std::string& name) const = 0;
    virtual block* begin_block(const std::string& name, function* func, const wall_e::str_vec& arg_names) = 0;
    virtual void end_block() = 0;
    //virtual void setup_insert_point() = 0;
    virtual function* begin_entry(const std::string& name) = 0;

    virtual void create_void_return() = 0;    //unit->llvm_builder()->CreateRetVoid();
    virtual void create_return(value* val) = 0;

    /**
     * @brief proto function creates function proto with unmangled symbol
     * @param name - name of symbol
     * @param arg_types - types of arguments
     * @param result_type - type of result
     * @param is_var_arg - is C varargs
     * @return llvm value
     */
    virtual function* proto(
            const std::string& name,
            wall_e::vec<type*> arg_types,
            type* result_type,
            bool is_var_arg = false
            ) = 0;


    virtual km2::backend::value* create_call(function* func, const wall_e::vec<value*>& args) = 0;

    virtual /* TODO SPECIFY TYPE */ void *inline_asm(const std::string &text) = 0;

    virtual void print() = 0;
    virtual void print_functions() = 0;
    virtual std::string llvm_assembly() const = 0;
    virtual wall_e::either<std::string, int> compile(const std::string &output_path) = 0;
    virtual wall_e::either<std::string, int> run_jit(function* entry_point) = 0;

    /**
     * @brief make_executable
     * @param output_path
     * @return
     * @note do not call in thread and dont ask why
     */
    virtual wall_e::either<std::string, int> make_executable(const std::string &output_path) = 0;

    //llvm::LLVMContext *llvm_context() const;
    //llvm::Module* llvm_module() const;
    //llvm::IRBuilder<> *llvm_builder() const;

    [[deprecated("dev function. will be removed")]]
    virtual int gen() = 0;
    [[deprecated("dev function. will be removed")]]
    virtual function* create_sum_function() = 0;
    [[deprecated("dev function. will be removed")]]
    virtual int aaa() = 0;

};

}
}

#endif // KM2_BACKEND_UNIT_H
