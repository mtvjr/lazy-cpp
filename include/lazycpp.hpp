#pragma once

#include <type_traits>
#include <variant>

namespace lazycpp {

namespace detail {

    /**
     * The LazyConcept defines the required and available
     * operations for lazy objects.
     */
    template <typename T>
    class LazyConcept {
    public:
        using value_type = T;

        LazyConcept() = default;
        virtual ~LazyConcept() = default;

        value_type& operator*() { return do_deref(); }

        const value_type& operator*() const { return do_deref(); }

        value_type& get_value() { return do_deref(); }

        const value_type& get_value() const { return do_deref(); }

        virtual bool isEvaluated() const = 0;

        void operator=(value_type& other) { do_reassign(other); }

        void operator=(value_type&& other) { do_reassign(other); }

    protected:
        virtual value_type& do_deref() = 0;
        virtual const value_type& do_deref() const = 0;
        virtual void do_reassign(value_type&& value) = 0;
        virtual void do_reassign(value_type& value) = 0;
    };

    /**
     * The NotLazy implementation of the LazyConcept hold a
     * non-lazy static value. It is useful for testing LazyConcepts.
     */
    template <typename T>
    class NotLazy : public LazyConcept<T> {
    public:
        using value_type = T;
        using LazyConcept<T>::operator=;

        explicit NotLazy(value_type t)
            : storage(std::move(t))
        {
        }

        bool isEvaluated() const override { return true; }

    protected:
        value_type& do_deref() override { return storage; }

        const value_type& do_deref() const override { return storage; }

        void do_reassign(value_type& value) override { storage = value; }

        void do_reassign(value_type&& value) override { storage = value; }

        value_type storage;
    };

    /**
     * The VariantLazy implementation of the lazy concept uses
     * std::variant to hold either the evaluator function or the
     * final variable.
     */
    template <typename T>
    class VariantLazy : public LazyConcept<T> {
    public:
        using value_type = T;
        using evaluator_type = std::function<T()>;
        using LazyConcept<T>::operator=;

        VariantLazy(evaluator_type evaluator)
            : storage(std::move(evaluator))
        {
        }

        bool isEvaluated() const override
        {
            return std::holds_alternative<value_type>(storage);
        }

    protected:
        value_type& do_deref() override
        {
            if (!isEvaluated()) {
                // Call the lazy function to evaluate the storage
                storage = std::get<evaluator_type>(storage)();
            }
            return std::get<value_type>(storage);
        }

        const value_type& do_deref() const override
        {
            if (!isEvaluated()) {
                // Call the lazy function to evaluate the storage
                storage = std::get<evaluator_type>(storage)();
            }
            return std::get<value_type>(storage);
        }

        void do_reassign(value_type&& value) override { storage = value; }

        void do_reassign(value_type& value) override { storage = value; }

        mutable std::variant<value_type, evaluator_type> storage;
    };

}

/**
 * SharedLazy is a wrapper for lazy objects that is backed
 * by std::shared_ptr. They provide read-only behavior and
 * can be useful for limiting the number of recalculations
 * required for a copied lazy variable.
 */
template <typename T>
class SharedLazy {
    using concept_type = detail::LazyConcept<T>;
    using ptr_type = std::shared_ptr<concept_type>;
    const ptr_type pimpl;

public:
    using value_type = T;

    SharedLazy(ptr_type ptr)
        : pimpl(std::move(ptr))
    {
    }

    const value_type& operator*() const { return pimpl->get_value(); }

    const value_type& get_value() const { return pimpl->get_value(); }

    bool isEvaluated() const { return pimpl->isEvaluated(); }
};

}