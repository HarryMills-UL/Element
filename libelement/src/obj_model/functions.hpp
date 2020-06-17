#pragma once

#ifndef LEGACY_COMPILER

#include <memory>
#include <unordered_map>

#include "ast/fwd.hpp"
#include "fwd.hpp"
#include "object.hpp"
#include "typeutil.hpp"

namespace element
{
    class intrinsic : public object, public rtti_type<intrinsic>
    {
    private:
        //TODO: this might need to be a constraint_const_shared_ptr
        type_const_shared_ptr return_type;

    public:
        intrinsic(element_type_id id, type_const_shared_ptr return_type);

    private:
        static const std::unordered_map<std::string, const std::shared_ptr<const intrinsic>> intrinsic_map;

    public:
        static std::shared_ptr<const intrinsic> get_intrinsic(const declaration& declaration);
    };

    class intrinsic_nullary final : public intrinsic
    {
    public:
        DECLARE_TYPE_ID();

    private:
        element_nullary_op operation;

    public:
        explicit intrinsic_nullary(element_nullary_op operation, type_const_shared_ptr return_type);

        [[nodiscard]] std::shared_ptr<element::object> call(const compilation_context& context, std::vector<std::shared_ptr<compiled_expression>> args) const override;
        [[nodiscard]] element_nullary_op get_operation() const { return operation; }
    };

    class intrinsic_unary final : public intrinsic
    {
    public:
        DECLARE_TYPE_ID();

    private:
        element_unary_op operation;
        //TODO: this might need to be a constraint_const_shared_ptr
        type_const_shared_ptr argument_type;

    public:
        explicit intrinsic_unary(element_unary_op operation, type_const_shared_ptr return_type, type_const_shared_ptr argument_type);

        [[nodiscard]] std::shared_ptr<element::object> call(const compilation_context& context, std::vector<std::shared_ptr<compiled_expression>> args) const override;
        [[nodiscard]] element_unary_op get_operation() const { return operation; }
    };

    class intrinsic_binary final : public intrinsic
    {
    public:
        DECLARE_TYPE_ID();

    private:
        element_binary_op operation;
        //TODO: this might need to be a constraint_const_shared_ptr
        type_const_shared_ptr first_argument_type;
        //TODO: this might need to be a constraint_const_shared_ptr
        type_const_shared_ptr second_argument_type;

    public:
        explicit intrinsic_binary(element_binary_op operation, type_const_shared_ptr return_type, type_const_shared_ptr first_argument_type, type_const_shared_ptr second_argument_type);

        [[nodiscard]] element_binary_op get_operation() const { return operation; }
        [[nodiscard]] std::shared_ptr<element::object> call(const compilation_context& context, std::vector<std::shared_ptr<compiled_expression>> args) const override;
    };

    ////TODO: Needs to be handled via list with dynamic indexing, this will be insufficient for when we have user input
    //class intrinsic_if final : public intrinsic
    //{
    //    DECLARE_TYPE_ID();

    //    intrinsic_if(type_const_shared_ptr return_type, std::string name)
    //        : intrinsic(type_id, std::move(return_type), std::move(name))
    //    {
    //    }
    //};
}

#else

#include <memory>
#include <string>

#include "ast/fwd.hpp"
#include "ast/scope.hpp"
#include "ast/types.hpp"
#include "construct.hpp"
#include "typeutil.hpp"

struct element_function : public element_construct, public rtti_type<element_function>
{
    // type_shared_ptr return_type() { return m_type; }
    type_const_shared_ptr return_type() const { return m_type; }

    virtual std::string name() const = 0;

    static function_const_shared_ptr get_builtin(const std::string& name);

protected:
    element_function(element_type_id id, type_const_shared_ptr return_type)
        : element_construct()
        , rtti_type(id)
        , m_type(std::move(return_type))
    {
    }

    void generate_ports_cache() const override;

    type_const_shared_ptr m_type;
};


struct element_intrinsic : public element_function
{
    DECLARE_TYPE_ID();

    std::string name() const override { return m_name; }

protected:
    element_intrinsic(element_type_id id, type_const_shared_ptr return_type, std::string name)
        : element_function(type_id | id, std::move(return_type))
        , m_name(std::move(name))
    {
    }

    std::string m_name;
};

struct element_intrinsic_nullary : public element_intrinsic
{
    DECLARE_TYPE_ID();

    element_intrinsic_nullary(element_nullary_op op, type_const_shared_ptr return_type, std::string name)
        : element_intrinsic(type_id, return_type, std::move(name))
        , m_op(op)
    {
    }

    element_nullary_op operation() const { return m_op; }

private:
    element_nullary_op m_op;
};

struct element_intrinsic_unary : public element_intrinsic
{
    DECLARE_TYPE_ID();

    element_intrinsic_unary(element_unary_op op, type_const_shared_ptr return_type, std::string name)
        : element_intrinsic(type_id, return_type, std::move(name))
        , m_op(op)
    {
    }

    element_unary_op operation() const { return m_op; }

private:
    element_unary_op m_op;
};

struct element_intrinsic_binary : public element_intrinsic
{
    DECLARE_TYPE_ID();

    element_intrinsic_binary(element_binary_op op, type_const_shared_ptr return_type, std::string name)
        : element_intrinsic(type_id, return_type, std::move(name))
        , m_op(op)
    {
    }

    element_binary_op operation() const { return m_op; }

private:
    element_binary_op m_op;
};

//TODO: Needs to be handled via list with dynamic indexing, this will be insufficient for when we have user input
struct element_intrinsic_if : public element_intrinsic {
    DECLARE_TYPE_ID();

    element_intrinsic_if(type_const_shared_ptr return_type, std::string name)
        : element_intrinsic(type_id, std::move(return_type), std::move(name))
    {
    }
};

// TOOD: expr groups et al


struct element_type_ctor : public element_function
{
    DECLARE_TYPE_ID();

    element_type_ctor(type_const_shared_ptr return_type)
        : element_function(type_id, std::move(return_type))
    {
    }

    std::string name() const override { return m_type->name(); }
};


struct element_custom_function : public element_function
{
    DECLARE_TYPE_ID();

    element_custom_function(const element_scope* scope)
        : element_function(type_id, generate_type(scope))
        , m_scope(scope)
    {
    }

    const element_scope* scope() const { return m_scope; }
    std::string name() const override { return scope()->name; }

protected:
    const element_scope* m_scope;

private:
    type_shared_ptr generate_type(const element_scope* scope) const;
};

#endif