#include "intrinsic_list.hpp"

//SELF
#include "object_model/declarations/struct_declaration.hpp"
#include "object_model/declarations/function_declaration.hpp"
#include "object_model/intermediaries/function_instance.hpp"
#include "object_model/error.hpp"

using namespace element;

intrinsic_list::intrinsic_list()
    : intrinsic_function(type_id, nullptr, true)
{
}

object_const_shared_ptr intrinsic_list::compile(const compilation_context& context,
                                                const source_information& source_info) const
{
    const auto& frame = context.calls.frames.back();
    const int count = frame.compiled_arguments.size();

    //todo: return errors instead of asserts
    assert(count != 0);

    const auto* list_declaration = context.get_global_scope()->find(identifier{ "List" }, false);
    assert(list_declaration);
    const auto* list_constructor = intrinsic::get_intrinsic(context.interpreter, list_declaration->identity);

    const auto* list_indexer_decl = context.get_compiler_scope()->find(identifier{ "@list_indexer" }, false);
    assert(list_indexer_decl);
    const auto* list_indexer_func = static_cast<const function_declaration*>(list_indexer_decl);
    assert(list_indexer_func);

    auto list_indexer = std::make_shared<function_instance>(list_indexer_func, context.captures, source_info);

    std::vector<object_const_shared_ptr> compiled_arguments;
    compiled_arguments.reserve(2);
    compiled_arguments.push_back(std::move(list_indexer));
    compiled_arguments.push_back(std::make_shared<const element_expression_constant>(count));
    return list_constructor->call(context, compiled_arguments, source_info);
}