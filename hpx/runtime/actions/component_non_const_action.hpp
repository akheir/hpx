//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file component_non_const_action.hpp

#if !defined(HPX_RUNTIME_ACTIONS_COMPONENT_NON_CONST_ACTION_MAR_26_2008_1054AM)
#define HPX_RUNTIME_ACTIONS_COMPONENT_NON_CONST_ACTION_MAR_26_2008_1054AM

#include <hpx/config/warnings_prefix.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace actions
{
    /// \cond NOINTERNAL

    ///////////////////////////////////////////////////////////////////////////
    //  Specialized generic component action types allowing to hold a different
    //  number of arguments
    ///////////////////////////////////////////////////////////////////////////

    // zero argument version
    template <
        typename Component, typename R,
        R (Component::*F)(), typename Derived>
    class component_base_action<R (Component::*)(), F, Derived>
      : public basic_action<Component, R(), Derived>
    {
    public:
        typedef basic_action<Component, R(), Derived> base_type;

        // Let the component decide whether the id is valid
        static bool is_target_valid(naming::id_type const& id)
        {
            return Component::is_target_valid(id);
        }

    protected:
        /// The \a continuation_thread_function will be registered as the thread
        /// function of a thread. It encapsulates the execution of the
        /// original function (given by \a func), while ignoring the return
        /// value.
        template <typename Address>   // dummy template parameter
        BOOST_FORCEINLINE static threads::thread_state_enum
        thread_function(Address lva)
        {
            try {
                LTM_(debug) << "Executing component action("
                            << detail::get_action_name<Derived>()
                            << ") lva(" << reinterpret_cast<void const*>
                                (get_lva<Component>::call(lva)) << ")";
                (get_lva<Component>::call(lva)->*F)();      // just call the function
            }
            catch (hpx::thread_interrupted const&) { //-V565
                /* swallow this exception */
            }
            catch (hpx::exception const& e) {
                LTM_(error)
                    << "Unhandled exception while executing component action("
                    << detail::get_action_name<Derived>()
                    << ") lva(" << reinterpret_cast<void const*>
                        (get_lva<Component>::call(lva)) << "): " << e.what();

                // report this error to the console in any case
                hpx::report_error(boost::current_exception());
            }
            catch (...) {
                LTM_(error)
                    << "Unhandled exception while executing component action("
                    << detail::get_action_name<Derived>()
                    << ") lva(" << reinterpret_cast<void const*>
                        (get_lva<Component>::call(lva)) << ")";

                // report this error to the console in any case
                hpx::report_error(boost::current_exception());
            }

            // Verify that there are no more registered locks for this
            // OS-thread. This will throw if there are still any locks
            // held.
            util::force_error_on_lock();
            return threads::terminated;
        }

    public:
        /// \brief This static \a construct_thread_function allows to construct
        /// a proper thread function for a \a thread without having to
        /// instantiate the \a component_base_action type. This is used by the \a
        /// applier in case no continuation has been supplied.
        template <typename Arguments>
        static threads::thread_function_type
        construct_thread_function(naming::address::address_type lva,
            Arguments && /*args*/)
        {
            threads::thread_state_enum (*f)(naming::address::address_type) =
                &Derived::template thread_function<naming::address::address_type>;

            return traits::action_decorate_function<Derived>::call(
                lva, util::bind(f, lva));
        }

        /// \brief This static \a construct_thread_function allows to construct
        /// a proper thread function for a \a thread without having to
        /// instantiate the \a component_base_action type. This is used by the \a
        /// applier in case a continuation has been supplied
        template <typename Arguments>
        static threads::thread_function_type
        construct_thread_function(continuation_type& cont,
            naming::address::address_type lva, Arguments && args)
        {
            return traits::action_decorate_function<Derived>::call(lva,
                base_type::construct_continuation_thread_object_function(
                    cont, F, get_lva<Component>::call(lva),
                    std::forward<Arguments>(args)));
        }

        // direct execution
        template <typename Arguments>
        BOOST_FORCEINLINE static R
        execute_function(naming::address::address_type lva,
            Arguments &&)
        {
            LTM_(debug)
                << "component_base_action::execute_function: name("
                << detail::get_action_name<Derived>()
                << ") lva(" << reinterpret_cast<void const*>(
                    get_lva<Component>::call(lva)) << ")";

            return (get_lva<Component>::call(lva)->*F)();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <
        typename Component, typename R,
        R (Component::*F)(), typename Derived>
    struct component_action<R (Component::*)(), F, Derived>
      : component_base_action<
            R (Component::*)(), F,
            typename detail::action_type<
                component_action<R (Component::*)(), F, Derived>, Derived
            >::type>
    {
        typedef typename detail::action_type<
            component_action, Derived
        >::type derived_type;

        typedef boost::mpl::false_ direct_execution;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <
        typename Component, typename R,
        R (Component::*F)(), typename Derived>
    struct make_action<R (Component::*)(), F, Derived, boost::mpl::false_>
      : component_action<R (Component::*)(), F, Derived>
    {
        typedef component_action<
            R (Component::*)(), F, Derived
        > type;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <
        typename Component, typename R,
        R (Component::*F)(), typename Derived>
    struct component_direct_action<R (Component::*)(), F, Derived>
      : public component_base_action<
            R (Component::*)(), F,
            typename detail::action_type<
                component_direct_action<R (Component::*)(), F, Derived>, Derived
            >::type>
    {
        typedef typename detail::action_type<
            component_direct_action, Derived
        >::type derived_type;

        typedef boost::mpl::true_ direct_execution;

        /// The function \a get_action_type returns whether this action needs
        /// to be executed in a new thread or directly.
        static base_action::action_type get_action_type()
        {
            return base_action::direct_action;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <
        typename Component, typename R,
        R (Component::*F)(), typename Derived>
    struct make_action<R (Component::*)(), F, Derived, boost::mpl::true_>
      : component_direct_action<R (Component::*)(), F, Derived>
    {
        typedef component_direct_action<
            R (Component::*)(), F, Derived
        > type;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  zero parameter version, no result value
    template <typename Component, void (Component::*F)(), typename Derived>
    class component_base_action<void (Component::*)(), F, Derived>
      : public basic_action<Component, util::unused_type(), Derived>
    {
    public:
        typedef basic_action<Component, util::unused_type(), Derived> base_type;

        // Let the component decide whether the id is valid
        static bool is_target_valid(naming::id_type const& id)
        {
            return Component::is_target_valid(id);
        }

    protected:
        /// The \a continuation_thread_function will be registered as the thread
        /// function of a thread. It encapsulates the execution of the
        /// original function (given by \a func), while ignoring the return
        /// value.
        template <typename Address>   // dummy template parameter
        BOOST_FORCEINLINE static threads::thread_state_enum
        thread_function(Address lva)
        {
            try {
                LTM_(debug) << "Executing component action("
                            << detail::get_action_name<Derived>()
                            << ") lva(" << reinterpret_cast<void const*>
                                (get_lva<Component>::call(lva)) << ")";
                (get_lva<Component>::call(lva)->*F)();      // just call the function
            }
            catch (hpx::thread_interrupted const&) { //-V565
                /* swallow this exception */
            }
            catch (hpx::exception const& e) {
                LTM_(error)
                    << "Unhandled exception while executing component action("
                    << detail::get_action_name<Derived>()
                    << ") lva(" << reinterpret_cast<void const*>
                        (get_lva<Component>::call(lva)) << "): " << e.what();

                // report this error to the console in any case
                hpx::report_error(boost::current_exception());
            }
            catch (...) {
                LTM_(error)
                    << "Unhandled exception while executing component action("
                    << detail::get_action_name<Derived>()
                    << ") lva(" << reinterpret_cast<void const*>
                        (get_lva<Component>::call(lva)) << ")";

                // report this error to the console in any case
                hpx::report_error(boost::current_exception());
            }

            // Verify that there are no more registered locks for this
            // OS-thread. This will throw if there are still any locks
            // held.
            util::force_error_on_lock();
            return threads::terminated;
        }

    public:
        /// \brief This static \a construct_thread_function allows to construct
        /// a proper thread function for a \a thread without having to
        /// instantiate the component_base_action type. This is used by the \a applier in
        /// case no continuation has been supplied.
        template <typename Arguments>
        static threads::thread_function_type
        construct_thread_function(naming::address::address_type lva,
            Arguments && /*args*/)
        {
            threads::thread_state_enum (*f)(naming::address::address_type) =
                &Derived::template thread_function<naming::address::address_type>;

            return traits::action_decorate_function<Derived>::call(
                lva, util::bind(f, lva));
        }

        /// \brief This static \a construct_thread_function allows to construct
        /// a proper thread function for a \a thread without having to
        /// instantiate the component_base_action type. This is used by the \a applier in
        /// case a continuation has been supplied
        template <typename Arguments>
        static threads::thread_function_type
        construct_thread_function(continuation_type& cont,
            naming::address::address_type lva, Arguments && args)
        {
            return traits::action_decorate_function<Derived>::call(lva,
                base_type::construct_continuation_thread_object_function_void(
                    cont, F, get_lva<Component>::call(lva),
                    std::forward<Arguments>(args)));
        }

        // direct execution
        template <typename Arguments>
        BOOST_FORCEINLINE static util::unused_type
        execute_function(naming::address::address_type lva,
            Arguments &&)
        {
            LTM_(debug)
                << "component_base_action::execute_function: name("
                << detail::get_action_name<Derived>()
                << ") lva(" << reinterpret_cast<void const*>(
                    get_lva<Component>::call(lva)) << ")";
            (get_lva<Component>::call(lva)->*F)();
            return util::unused;
        }
    };

    /// \endcond
}}

/// \def HPX_DEFINE_COMPONENT_ACTION(component, func, action_type)
///
/// \brief Registers a non-const member function of a component as an action type
/// with HPX
///
/// The macro \a HPX_DEFINE_COMPONENT_CONST_ACTION can be used to register a
/// non-const member function of a component as an action type named \a action_type.
///
/// The parameter \a component is the type of the component exposing the
/// non-const member function \a func which should be associated with the newly
/// defined action type. The parameter \p action_type is the name of the action
/// type to register with HPX.
///
/// \par Example:
///
/// \code
///       namespace app
///       {
///           // Define a simple component exposing one action 'print_greating'
///           class HPX_COMPONENT_EXPORT server
///             : public hpx::components::simple_component_base<server>
///           {
///               void print_greating ()
///               {
///                   hpx::cout << "Hey, how are you?\n" << hpx::flush;
///               }
///
///               // Component actions need to be declared, this also defines the
///               // type 'print_greating_action' representing the action.
///               HPX_DEFINE_COMPONENT_ACTION(server, print_greating, print_greating_action);
///           };
///       }
/// \endcode
///
/// \note This macro should be used for non-const member functions only. Use
/// the macro \a HPX_DEFINE_COMPONENT_CONST_ACTION for const member functions.
///
/// The first argument must provide the type name of the component the
/// action is defined for.
///
/// The second argument must provide the member function name the action
/// should wrap.
///
/// \note The macro \a HPX_DEFINE_COMPONENT_ACTION can be used with 2 or
/// 3 arguments. The third argument is optional.
///
/// The default value for the third argument (the typename of the defined
/// action) is derived from the name of the function (as passed as the second
/// argument) by appending '_action'. The third argument can be omitted only
/// if the first argument with an appended suffix '_action' resolves to a valid,
/// unqualified C++ type name.
///
#define HPX_DEFINE_COMPONENT_ACTION(...)                                      \
    HPX_DEFINE_COMPONENT_ACTION_(__VA_ARGS__)                                 \
    /**/

/// \cond NOINTERNAL
#define HPX_DEFINE_COMPONENT_ACTION_(...)                                     \
    HPX_UTIL_EXPAND_(BOOST_PP_CAT(                                            \
        HPX_DEFINE_COMPONENT_ACTION_, HPX_UTIL_PP_NARG(__VA_ARGS__)           \
    )(__VA_ARGS__))                                                           \
    /**/

#define HPX_DEFINE_COMPONENT_ACTION_2(component, func)                        \
    typedef HPX_MAKE_COMPONENT_ACTION(component, func)::type                  \
        BOOST_PP_CAT(func, _action)                                           \
    /**/
#define HPX_DEFINE_COMPONENT_ACTION_3(component, func, action_type)           \
    typedef HPX_MAKE_COMPONENT_ACTION(component, func)::type action_type      \
    /**/
/// \endcond

/// \cond NOINTERNAL
#define HPX_DEFINE_COMPONENT_DIRECT_ACTION(...)                               \
    HPX_DEFINE_COMPONENT_DIRECT_ACTION_(__VA_ARGS__)                          \
    /**/

#define HPX_DEFINE_COMPONENT_DIRECT_ACTION_(...)                              \
    HPX_UTIL_EXPAND_(BOOST_PP_CAT(                                            \
        HPX_DEFINE_COMPONENT_DIRECT_ACTION_, HPX_UTIL_PP_NARG(__VA_ARGS__)    \
    )(__VA_ARGS__))                                                           \
    /**/

#define HPX_DEFINE_COMPONENT_DIRECT_ACTION_2(component, func)                 \
    typedef HPX_MAKE_DIRECT_COMPONENT_ACTION(component, func)::type           \
        BOOST_PP_CAT(func, _action)                                           \
    /**/
#define HPX_DEFINE_COMPONENT_DIRECT_ACTION_3(component, func, action_type)    \
    typedef HPX_MAKE_DIRECT_COMPONENT_ACTION(component, func)::type           \
        action_type                                                           \
    /**/

///////////////////////////////////////////////////////////////////////////////
// same as above, just for template functions
#define HPX_DEFINE_COMPONENT_ACTION_TPL(...)                                  \
    HPX_DEFINE_COMPONENT_ACTION_TPL_(__VA_ARGS__)                             \
    /**/

#define HPX_DEFINE_COMPONENT_ACTION_TPL_(...)                                 \
    HPX_UTIL_EXPAND_(BOOST_PP_CAT(                                            \
        HPX_DEFINE_COMPONENT_ACTION_TPL_, HPX_UTIL_PP_NARG(__VA_ARGS__)       \
    )(__VA_ARGS__))                                                           \
    /**/

#define HPX_DEFINE_COMPONENT_ACTION_TPL_2(component, func)                    \
    typedef typename HPX_MAKE_COMPONENT_ACTION_TPL(component, func)::type     \
        BOOST_PP_CAT(func, _action)                                           \
    /**/
#define HPX_DEFINE_COMPONENT_ACTION_TPL_3(component, func, action_type)       \
    typedef typename HPX_MAKE_COMPONENT_ACTION_TPL(component, func)::type     \
        action_type                                                           \
    /**/

///////////////////////////////////////////////////////////////////////////////
#define HPX_DEFINE_COMPONENT_DIRECT_ACTION_TPL(...)                           \
    HPX_DEFINE_COMPONENT_DIRECT_ACTION_TPL_(__VA_ARGS__)                      \
    /**/

#define HPX_DEFINE_COMPONENT_DIRECT_ACTION_TPL_(...)                          \
    HPX_UTIL_EXPAND_(BOOST_PP_CAT(                                            \
        HPX_DEFINE_COMPONENT_DIRECT_ACTION_TPL_, HPX_UTIL_PP_NARG(__VA_ARGS__)\
    )(__VA_ARGS__))                                                           \
    /**/

#define HPX_DEFINE_COMPONENT_DIRECT_ACTION_TPL_2(component, func)             \
    typedef typename                                                          \
        HPX_MAKE_DIRECT_COMPONENT_ACTION_TPL(component, func)::type           \
        BOOST_PP_CAT(func, _action)                                           \
    /**/
#define HPX_DEFINE_COMPONENT_DIRECT_ACTION_TPL_3(component, func, action_type)\
    typedef typename                                                          \
        HPX_MAKE_DIRECT_COMPONENT_ACTION_TPL(component, func)::type           \
        action_type                                                           \
    /**/
/// \endcond

/// \def HPX_DEFINE_COMPONENT_CONST_ACTION(component, func, action_type)
///
/// \brief Registers a const member function of a component as an action type
/// with HPX
///
/// The macro \a HPX_DEFINE_COMPONENT_CONST_ACTION can be used to register a
/// const member function of a component as an action type named \a action_type.
///
/// The parameter \a component is the type of the component exposing the const
/// member function \a func which should be associated with the newly defined
/// action type. The parameter \p action_type is the name of the action type to
/// register with HPX.
///
/// \par Example:
///
/// \code
///       namespace app
///       {
///           // Define a simple component exposing one action 'print_greating'
///           class HPX_COMPONENT_EXPORT server
///             : public hpx::components::simple_component_base<server>
///           {
///               void print_greating() const
///               {
///                   hpx::cout << "Hey, how are you?\n" << hpx::flush;
///               }
///
///               // Component actions need to be declared, this also defines the
///               // type 'print_greating_action' representing the action.
///               HPX_DEFINE_COMPONENT_CONST_ACTION(server, print_greating, print_greating_action);
///           };
///       }
/// \endcode
///
/// \note This macro should be used for const member functions only. Use
/// the macro \a HPX_DEFINE_COMPONENT_ACTION for non-const member functions.
///
/// The first argument must provide the type name of the component the
/// action is defined for.
///
/// The second argument must provide the member function name the action
/// should wrap.
///
/// \note The macro \a HPX_DEFINE_COMPONENT_CONST_ACTION can be used with 2 or
/// 3 arguments. The third argument is optional.
///
/// The default value for the third argument (the typename of the defined
/// action) is derived from the name of the function (as passed as the second
/// argument) by appending '_action'. The third argument can be omitted only
/// if the first argument with an appended suffix '_action' resolves to a valid,
/// unqualified C++ type name.
///
#define HPX_DEFINE_COMPONENT_CONST_ACTION(...)                                \
    HPX_DEFINE_COMPONENT_CONST_ACTION_(__VA_ARGS__)                           \
    /**/

/// \cond NOINTERNAL
#define HPX_DEFINE_COMPONENT_CONST_ACTION_(...)                               \
    HPX_UTIL_EXPAND_(BOOST_PP_CAT(                                            \
        HPX_DEFINE_COMPONENT_CONST_ACTION_, HPX_UTIL_PP_NARG(__VA_ARGS__)     \
    )(__VA_ARGS__))                                                           \
    /**/

#define HPX_DEFINE_COMPONENT_CONST_ACTION_2(component, func)                  \
    typedef HPX_MAKE_CONST_COMPONENT_ACTION(component, func)::type            \
        BOOST_PP_CAT(func, _action)                                           \
    /**/
#define HPX_DEFINE_COMPONENT_CONST_ACTION_3(component, func, action_type)     \
    typedef HPX_MAKE_CONST_COMPONENT_ACTION(component, func)::type action_type\
    /**/
/// \endcond

/// \cond NOINTERNAL
#define HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION(...)                         \
    HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION_(__VA_ARGS__)                    \
    /**/

#define HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION_(...)                        \
    HPX_UTIL_EXPAND_(BOOST_PP_CAT(                                            \
        HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION_,                            \
            HPX_UTIL_PP_NARG(__VA_ARGS__)                                     \
    )(__VA_ARGS__))                                                           \
    /**/

#define HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION_2(component, func)           \
    typedef HPX_MAKE_CONST_DIRECT_COMPONENT_ACTION(component, func)::type     \
        BOOST_PP_CAT(func, _action)                                           \
    /**/
#define HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION_3(component, func,           \
        action_type)                                                          \
    typedef HPX_MAKE_CONST_DIRECT_COMPONENT_ACTION(component, func)::type     \
        action_type                                                           \
    /**/

///////////////////////////////////////////////////////////////////////////////
// same as above, just for template functions
#define HPX_DEFINE_COMPONENT_CONST_ACTION_TPL(...)                            \
    HPX_DEFINE_COMPONENT_CONST_ACTION_TPL_(__VA_ARGS__)                       \
    /**/

#define HPX_DEFINE_COMPONENT_CONST_ACTION_TPL_(...)                           \
    HPX_UTIL_EXPAND_(BOOST_PP_CAT(                                            \
        HPX_DEFINE_COMPONENT_CONST_ACTION_TPL_, HPX_UTIL_PP_NARG(__VA_ARGS__) \
    )(__VA_ARGS__))                                                           \
    /**/

#define HPX_DEFINE_COMPONENT_CONST_ACTION_TPL_2(component, func)              \
    typedef typename                                                          \
        HPX_MAKE_CONST_COMPONENT_ACTION_TPL(component, func)::type            \
        BOOST_PP_CAT(func, _action)                                           \
    /**/
#define HPX_DEFINE_COMPONENT_CONST_ACTION_TPL_3(component, func, action_type) \
    typedef typename                                                          \
        HPX_MAKE_CONST_COMPONENT_ACTION_TPL(component, func)::type            \
        action_type                                                           \
    /**/

///////////////////////////////////////////////////////////////////////////////
#define HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION_TPL(...)                     \
    HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION_TPL_(__VA_ARGS__)                \
    /**/

#define HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION_TPL_(...)                    \
    HPX_UTIL_EXPAND_(BOOST_PP_CAT(                                            \
        HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION_TPL_,                        \
            HPX_UTIL_PP_NARG(__VA_ARGS__)                                     \
    )(__VA_ARGS__))                                                           \
    /**/

#define HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION_TPL_2(component, func)       \
    typedef typename                                                          \
        HPX_MAKE_CONST_DIRECT_COMPONENT_ACTION_TPL(component, func)::type     \
        BOOST_PP_CAT(func, _action)                                           \
    /**/
#define HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION_TPL_3(component, func,       \
        action_type)                                                          \
    typedef typename                                                          \
        HPX_MAKE_CONST_DIRECT_COMPONENT_ACTION_TPL(component, func)::type     \
        action_type                                                           \
    /**/
/// \endcond

#include <hpx/config/warnings_suffix.hpp>

#endif

