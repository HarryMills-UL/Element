#pragma once

//SELF
#include "expression.hpp"
#include "object_model/object.hpp"
#include "object_model/capture_stack.hpp"

namespace element
{
    class expression_chain final : public object
    {
    public:
        explicit expression_chain(const declaration* declarer);

        [[nodiscard]] std::string typeof_info() const override;
        [[nodiscard]] std::string to_code(int depth = 0) const override;
        [[nodiscard]] object_const_shared_ptr call(const compilation_context& context,
                                                   std::vector<object_const_shared_ptr> compiled_args,
                                                   const source_information& source_info) const override;

        [[nodiscard]] object_const_shared_ptr compile(const compilation_context& context,
                                                            const source_information& source_info) const override;

        const declaration* declarer;
        std::vector<std::unique_ptr<expression>> expressions;
        mutable capture_stack captures;
    private:
    };
}