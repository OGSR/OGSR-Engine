// Copyright (c) 2003 Daniel Wallin and Arvid Norberg

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

/*
	ISSUES:
	------------------------------------------------------

	* solved for member functions, not application operator *
	if we have a base class that defines a function a derived class must be able to
	override that function (not just overload). Right now we just add the other overload
	to the overloads list and will probably get an ambiguity. If we want to support this
	each method_rep must include a vector of type_info pointers for each parameter.
	Operators do not have this problem, since operators always have to have
	it's own type as one of the arguments, no ambiguity can occur. Application
	operator, on the other hand, would have this problem.
	Properties cannot be overloaded, so they should always be overridden.
	If this is to work for application operator, we really need to specify if an application
	operator is const or not.

	If one class registers two functions with the same name and the same
	signature, there's currently no error. The last registered function will
	be the one that's used.
	How do we know which class registered the function? If the function was
	defined by the base class, it is a legal operation, to override it.
	we cannot look at the pointer offset, since it always will be zero for one of the bases.



	TODO:
	------------------------------------------------------

 	finish smart pointer support
		* the adopt policy should not be able to adopt pointers to held_types. This
		must be prohibited.
		* name_of_type must recognize holder_types and not return "custom"

	document custom policies, custom converters

	store the instance object for policies.

	support the __concat metamethod. This is a bit tricky, since it cannot be
	treated as a normal operator. It is a binary operator but we want to use the
	__tostring implementation for both arguments.
	
*/

#include <luabind/config.hpp>

#include <string>
#include <map>
#include <vector>
#include <cassert>

#include <Utils/imdexlib/mpl.hpp>
#include <Utils/imdexlib/utility.hpp>

#include <luabind/config.hpp>
#include <luabind/scope.hpp>
#include <luabind/back_reference.hpp>
#include <luabind/detail/constructor.hpp>
#include <luabind/detail/call.hpp>
#include <luabind/detail/signature_match.hpp>
#include <luabind/detail/primitives.hpp>
#include <luabind/detail/property.hpp>
#include <luabind/detail/typetraits.hpp>
#include <luabind/detail/class_rep.hpp>
#include <luabind/detail/method_rep.hpp>
#include <luabind/detail/construct_rep.hpp>
#include <luabind/detail/object_rep.hpp>
#include <luabind/detail/calc_arity.hpp>
#include <luabind/detail/call_member.hpp>
#include <luabind/detail/enum_maker.hpp>
#include <luabind/detail/get_signature.hpp>
#include <luabind/detail/implicit_cast.hpp>
#include <luabind/detail/operator_id.hpp>
#include <luabind/detail/pointee_typeid.hpp>
#include <luabind/detail/link_compatibility.hpp>
#include <luabind/raw_policy.hpp>

// to remove the 'this' used in initialization list-warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355)
#endif

namespace luabind
{	
	namespace detail
	{
		template<class Derived> struct operator_;
	}

	using detail::type;

	template<class T, typename... Xs>
	struct class_;

/*    template <typename... Ts>
    detail::null_type* get_const_holder(Ts&&...)
    {
        static_assert(imdexlib::false_v<Ts...>, "this function will only be invoked if the user hasn't defined a correct overload");
        return nullptr;
    }*/
    // TODO: this function will only be invoked if the user hasn't defined a correct overload
    // maybe we should have a static assert in here?
    inline detail::null_type* get_const_holder(...)
    {
        return nullptr;
    }

	namespace detail
	{
		template<typename T>
		struct is_bases : public std::false_type
		{
		};

        template<typename... Ts>
        struct is_bases<bases<Ts...>> : public std::true_type
        {
        };

        template <typename Predicate>
        struct get_predicate
        {
            template <typename T>
            using type = typename Predicate::template type<T>;
        };

        template <typename Pred, typename DefaultValue, typename... Parameters>
        struct extract_parameter
        {
            using pred = get_predicate<Pred>;
            using iterator = imdexlib::find_if_t<pred, Parameters...>;

            using type = std::conditional_t<
                std::is_same_v<iterator, imdexlib::end_t>,
                DefaultValue,
                iterator
            >;
        };

		template<typename Fn, typename Class, typename... Policies>
		struct mem_fn_callback
		{
			typedef int result_type;

			int operator()(lua_State* L) const
			{
				return call(fn, static_cast<Class*>(nullptr), L, policy_cons<Policies...>());
			}

			mem_fn_callback(Fn fn_)
				: fn(fn_)
			{
			}

			Fn fn;
		};

		template<typename Fn, typename Class, typename... Policies>
		struct mem_fn_matcher
		{
			typedef int result_type;

			int operator()(lua_State* L) const
			{
				return match(fn, L, static_cast<Class*>(nullptr), policy_cons<Policies...>());
			}

			mem_fn_matcher(Fn fn_)
				: fn(fn_)
			{
			}

			Fn fn;
		};

		struct pure_virtual_tag
		{
			static void precall(lua_State*, index_map const&) {}
			static void postcall(lua_State*, index_map const&) {}
		};

		template<typename... Policies>
        struct has_pure_virtual;

        template<typename T>
        struct has_pure_virtual<T> : public std::is_same<pure_virtual_tag, T>
        {
        };

        template<typename T, typename... Policies>
        struct has_pure_virtual<T, Policies...> : public std::conditional_t<
                                                            std::is_same_v<pure_virtual_tag, T>,
                                                            std::true_type,
                                                            has_pure_virtual<Policies...>
	                                                     >
		{
		};

		template<>
		struct has_pure_virtual<> : public std::false_type
		{
		};

		// prints the types of the values on the stack, in the
		// range [start_index, lua_gettop()]

		LUABIND_API string_class stack_content_by_name(lua_State* L, int start_index);
	
		struct LUABIND_API create_class
		{
			static int stage1(lua_State* L);
			static int stage2(lua_State* L);
		};

		// if the class is held by a smart pointer, we need to be able to
		// implicitly dereference the pointer when needed.

		template<typename UnderlyingT, typename HeldT>
		struct extract_underlying_type
		{
			static void* extract(void* ptr)
			{
				HeldT& held_obj = *reinterpret_cast<HeldT*>(ptr);
				UnderlyingT* underlying_ptr = static_cast<UnderlyingT*>(get_pointer(held_obj));
				return underlying_ptr;
			}
		};

		template<typename UnderlyingT, typename HeldT>
		struct extract_underlying_const_type
		{
			static const void* extract(void* ptr)
			{
				HeldT& held_obj = *reinterpret_cast<HeldT*>(ptr);
				const UnderlyingT* underlying_ptr = static_cast<const UnderlyingT*>(get_pointer(held_obj));
				return underlying_ptr;
			}
		};

		template<typename HeldType>
		struct internal_holder_extractor
		{
			typedef void*(*extractor_fun)(void*);

			template<class T>
			static extractor_fun apply(detail::type<T>)
			{
				return &detail::extract_underlying_type<T, HeldType>::extract;
			}
		};

		template<>
		struct internal_holder_extractor<detail::null_type>
		{
			typedef void*(*extractor_fun)(void*);

			template<class T>
			static extractor_fun apply(detail::type<T>)
			{
				return nullptr;
			}
		};


		template<typename HeldType, typename ConstHolderType>
		struct convert_holder
		{
			static void apply(void* holder, void* target)
			{
				new(target) ConstHolderType(*reinterpret_cast<HeldType*>(holder));
			};
		};


		template<typename HeldType>
		struct const_converter
		{
			typedef void(*converter_fun)(void*, void*);

			template<class ConstHolderType>
			static converter_fun apply(ConstHolderType*)
			{
				return &detail::convert_holder<HeldType, ConstHolderType>::apply;
			}
		};

		template<>
		struct const_converter<detail::null_type>
		{
			typedef void(*converter_fun)(void*, void*);

			template<class T>
			static converter_fun apply(T*)
			{
				return nullptr;
			}
		};




		template<typename HeldType>
		struct internal_const_holder_extractor
		{
			typedef const void*(*extractor_fun)(void*);

			template<class T>
			static extractor_fun apply(detail::type<T>)
			{
				return get_extractor(detail::type<T>(), luabind::get_const_holder(static_cast<HeldType*>(nullptr)));
			}
		private:
			template<class T, class ConstHolderType>
			static extractor_fun get_extractor(detail::type<T>, ConstHolderType*)
			{
				return &detail::extract_underlying_const_type<T, ConstHolderType>::extract;
			}
		};

		template<>
		struct internal_const_holder_extractor<detail::null_type>
		{
			typedef const void*(*extractor_fun)(void*);

			template<class T>
			static extractor_fun apply(detail::type<T>)
			{
				return nullptr;
			}
		};



		// this is simply a selector that returns the type_info
		// of the held type, or invalid_type_info if we don't have
		// a held_type
		template<typename HeldType>
		struct internal_holder_type
		{
			static LUABIND_TYPE_INFO apply()
			{
				return LUABIND_TYPEID(HeldType);
			}
		};

		template<>
		struct internal_holder_type<detail::null_type>
		{
			static LUABIND_TYPE_INFO apply()
			{
				return LUABIND_INVALID_TYPE_INFO;
			}
		};


		// this is the actual held_type constructor
		template<typename HeldType, typename T>
		struct internal_construct_holder
		{
			static void apply(void* target, void* raw_pointer)
			{
				new(target) HeldType(static_cast<T*>(raw_pointer));
			}
		};

		// this is the actual held_type default constructor
		template<typename HeldType, typename T>
		struct internal_default_construct_holder
		{
			static void apply(void* target)
			{
				new(target) HeldType();
			}
		};

		// the following two functions are the ones that returns
		// a pointer to a held_type_constructor, or 0 if there
		// is no held_type
		template<typename HeldType>
		struct holder_constructor
		{
			typedef void(*constructor)(void*,void*);
			template<typename T>
			static constructor apply(detail::type<T>)
			{
				return &internal_construct_holder<HeldType, T>::apply;
			}
		};

		template<>
		struct holder_constructor<detail::null_type>
		{
			typedef void(*constructor)(void*,void*);
			template<typename T>
			static constructor apply(detail::type<T>)
			{
				return nullptr;
			}
		};

		// the following two functions are the ones that returns
		// a pointer to a const_held_type_constructor, or 0 if there
		// is no held_type
		template<typename HolderType>
		struct const_holder_constructor
		{
			typedef void(*constructor)(void*,void*);
			template<typename T>
			static constructor apply(detail::type<T>)
			{
				return get_const_holder_constructor(detail::type<T>(), luabind::get_const_holder(static_cast<HolderType*>(nullptr)));
			}

		private:

			template<typename T, typename ConstHolderType>
				static constructor get_const_holder_constructor(detail::type<T>, ConstHolderType*)
			{
				return &internal_construct_holder<ConstHolderType, T>::apply;
			}
		};

		template<>
		struct const_holder_constructor<detail::null_type>
		{
			typedef void(*constructor)(void*,void*);
			template<typename T>
			static constructor apply(detail::type<T>)
			{
				return nullptr;
			}
		};


		// the following two functions are the ones that returns
		// a pointer to a held_type_constructor, or 0 if there
		// is no held_type. The holder_type is default constructed
		template<typename HeldType>
		struct holder_default_constructor
		{
			typedef void(*constructor)(void*);
			template<typename T>
			static constructor apply(detail::type<T>)
			{
				return &internal_default_construct_holder<HeldType, T>::apply;
			}
		};

		template<>
		struct holder_default_constructor<detail::null_type>
		{
			typedef void(*constructor)(void*);
			template<typename T>
			static constructor apply(detail::type<T>)
			{
				return nullptr;
			}
		};


		// the following two functions are the ones that returns
		// a pointer to a const_held_type_constructor, or 0 if there
		// is no held_type. The constructed held_type is default
		// constructed
		template<typename HolderType>
		struct const_holder_default_constructor
		{
			typedef void(*constructor)(void*);
			template<typename T>
			static constructor apply(detail::type<T>)
			{
				return get_const_holder_default_constructor(detail::type<T>(), luabind::get_const_holder(static_cast<HolderType*>(nullptr)));
			}

		private:

			template<typename T, typename ConstHolderType>
			static constructor get_const_holder_default_constructor(detail::type<T>, ConstHolderType*)
			{
				return &internal_default_construct_holder<ConstHolderType, T>::apply;
			}
		};

		template<>
		struct const_holder_default_constructor<detail::null_type>
		{
			typedef void(*constructor)(void*);
			template<typename T>
			static constructor apply(detail::type<T>)
			{
				return nullptr;
			}
		};




		// this is a selector that returns the size of the held_type
		// or 0 if we don't have a held_type
		template <typename HolderType>
		struct internal_holder_size
		{
			static int apply() { return get_internal_holder_size(luabind::get_const_holder(static_cast<HolderType*>(nullptr))); }
		private:
			template<typename ConstHolderType>
			static int get_internal_holder_size(ConstHolderType*)
			{
				return std::max(sizeof(HolderType), sizeof(ConstHolderType));
			}
		};

		template <>
		struct internal_holder_size<detail::null_type>
		{
			static int apply() {	return 0; }
		};


		// if we have a held type, return the destructor to it
		// note the difference. The held_type should only be destructed (not deleted)
		// since it's constructed in the lua userdata
		template<typename HeldType>
		struct internal_holder_destructor
		{
			typedef void(*destructor_t)(void*);
			template<typename T>
			static destructor_t apply(detail::type<T>)
			{
				return &detail::destruct_only_s<HeldType>::apply;
			}
		};

		// if we don't have a held type, return the destructor of the raw type
		template<>
		struct internal_holder_destructor<detail::null_type>
		{
			typedef void(*destructor_t)(void*);
			template<typename T>
			static destructor_t apply(detail::type<T>)
			{
				return &detail::delete_s<T>::apply;
			}
		};

		
		// if we have a held type, return the destructor to it's const version
		template<typename HolderType>
		struct internal_const_holder_destructor
		{
			typedef void(*destructor_t)(void*);
			template<typename T>
			static destructor_t apply(detail::type<T>)
			{
				return const_holder_type_destructor(luabind::get_const_holder(static_cast<HolderType*>(0)));
			}

		private:

			template<typename ConstHolderType>
			static destructor_t const_holder_type_destructor(ConstHolderType*)
			{
				return &detail::destruct_only_s<ConstHolderType>::apply;
			}

		};

		// if we don't have a held type, return the destructor of the raw type
		template<>
		struct internal_const_holder_destructor<detail::null_type>
		{
			typedef void(*destructor_t)(void*);
			template<typename T>
			static destructor_t apply(detail::type<T>)
			{
				return nullptr;
			}
		};




		template<typename HolderType>
		struct get_holder_alignment
		{
			static int apply()
			{
				return internal_alignment(luabind::get_const_holder(static_cast<HolderType*>(nullptr)));
			}

		private:

			template<typename ConstHolderType>
			static int internal_alignment(ConstHolderType*)
			{
				return std::max(alignof(HolderType), alignof(ConstHolderType));
			}
		};

		template<>
		struct get_holder_alignment<detail::null_type>
		{
			static int apply()
			{
				return 1;
			}
		};


	} // detail

	namespace detail {

        template <typename T>
        struct static_scope
        {
            static_scope(T&& self)
                : self(std::move(self))
            {
            }

            T operator[](scope&& s) &&
            {
                self.add_inner_scope(std::move(s));
                return std::move(self);
            }

            static_scope(const static_scope&) = delete;

            static_scope(static_scope&& that) noexcept
                : self(std::move(that.self))
            {
            }

            static_scope& operator= (const static_scope&) = delete;

            static_scope& operator= (static_scope&& that) noexcept
            {
                self = std::move(that);
                return *this;
            }

        private:

            T self;
        };

		struct class_registration;

		struct LUABIND_API class_base : scope
		{
		public:
			class_base(char const* name);

            class_base(const class_base&) = delete;
            class_base(class_base&& that) noexcept
                : scope(std::move(that)),
                  m_registration(that.m_registration)
            {
                that.m_registration = nullptr;
            }

            class_base& operator= (const class_base&) = delete;

            class_base& operator= (class_base&& that) noexcept
            {
                scope::operator=(std::move(that));
                std::swap(m_registration, that.m_registration);
                return *this;
            }

			struct base_desc
			{
				LUABIND_TYPE_INFO type;
				int ptr_offset;
			};

			void init(
				LUABIND_TYPE_INFO type
				, LUABIND_TYPE_INFO holder_type
				, LUABIND_TYPE_INFO const_holder_type
				, void*(*extractor)(void*)
				, const void*(*const_extractor)(void*)
				, void(*const_converter)(void*,void*)
				, void(*holder_constructor)(void*,void*)
				, void(*const_holder_constructor)(void*,void*)
				, void(*holder_default_constructor)(void*)
				, void(*const_holder_default_constructor)(void*)
				, void(*destructor)(void*)
				, void(*const_holder_destructor)(void*)
				, void(*m_adopt_fun)(void*)
				, int holder_size
				, int holder_alignment);

			void add_getter(const char* name, std::function<int(lua_State*, int)> g);

#ifdef LUABIND_NO_ERROR_CHECKING
			void class_base::add_setter(
				const char* name
				, std::function<int(lua_State*, int)> s);
#else
			void class_base::add_setter(
				const char* name
				, std::function<int(lua_State*, int)> s
				, int (*match)(lua_State*, int)
				, void (*get_sig_ptr)(lua_State*, string_class&));
#endif

			void add_base(const base_desc& b);
            void add_constructor(const detail::construct_rep::overload_t& o);
			void add_constructor(detail::construct_rep::overload_t&& o);	
			void add_method(const char* name, const detail::overload_rep& o);
            void add_method(const char* name, detail::overload_rep&& o);

#ifndef LUABIND_NO_ERROR_CHECKING
			void add_operator(
				int op_id
				,  int(*func)(lua_State*)
				, int(*matcher)(lua_State*)
				, void(*sig)(lua_State*
				, string_class&)
				, int arity);
#else
			void add_operator(
				int op_id
				,  int(*func)(lua_State*)
				, int(*matcher)(lua_State*)
				, int arity);
#endif

			const char* name() const;

			void add_static_constant(const char* name, int val);
			void add_inner_scope(scope&& s);

		private:
            class_base(class_registration*);

			class_registration* m_registration;
		};
	
        template<typename T, typename W>
        struct adopt_function
		{
		    static void execute(void* p)
            {
			    wrapped_self_t& self = wrap_access::ref(
					*static_cast<W*>(static_cast<T*>(p))
				);

				LUABIND_CHECK_STACK(self.state());

				self.get(self.state());
				self.m_strong_ref.set(self.state());
            }
        };

	} // namespace detail

	// registers a class in the lua environment
	template<typename T, typename... Xs>
	struct class_: detail::class_base 
	{
	    using self_t = class_<T, Xs...>;

        class_(const class_&) = delete;

        class_(class_&& that) noexcept
            : class_base(std::move(that))
        {
        }

        class_& operator= (const class_&) = delete;

        class_& operator= (class_&& that) noexcept
        {
            detail::class_base::operator=(std::move(that));
            return *this;
        }

	private:

        struct WrappedTypePredicate
        {
            template <typename U>
            using type = std::is_base_of<T, U>;
        };

        struct HeldTypePredicate
        {
            template <typename U>
            using type = std::negation<
                std::disjunction<
                    detail::is_bases<U>,
                    std::is_base_of<U, T>,
                    std::is_base_of<T, U>
                >
            >;
        };

	public:

        // WrappedType MUST inherit from T
        using WrappedType = typename detail::extract_parameter<
            WrappedTypePredicate,
            detail::null_type,
            Xs...
        >::type;

        using HeldType = typename detail::extract_parameter<
            HeldTypePredicate,
            detail::null_type,
            Xs...
        >::type;

		// this function generates conversion information
		// in the given class_rep structure. It will be able
		// to implicitly cast to the given template type
		template<typename To>
		void gen_base_info()
		{
			// fist, make sure the given base class is registered.
			// if it's not registered we can't push it's lua table onto
			// the stack because it doesn't have a table

			// try to cast this type to the base type and remember
			// the pointer offset. For multiple inheritance the pointer
			// may change when casting. Since we need to be able to
			// cast we need this pointer offset.
			// store the information in this class' base class-vector
			base_desc base;
			base.type = LUABIND_TYPEID(To);
			base.ptr_offset = detail::ptr_offset(detail::type<T>(), detail::type<To>());
			add_base(base);
		}

		template<typename... Bs>
		void generate_baseclass_list(const detail::type<bases<Bs...>>)
		{
            const int expander [] = { 0, (gen_base_info<Bs>(), 0)... };
            (void) expander;
		}

		class_(const char* name): class_base(name)
		{
#ifndef NDEBUG
			detail::check_link_compatibility();
#endif
		   	init(); 
		}

		template<typename F>
		class_&& def(const char* name, const F f) &&
		{
			return std::move(*this).virtual_def(name, f, detail::policy_cons<>(), detail::policy_cons<>(), std::true_type());
		}

		// virtual functions
		template<typename F, typename DefaultOrPolicies>
		class_&& def(char const* name, const F fn, DefaultOrPolicies default_or_policies) &&
		{
			return std::move(*this).virtual_def(name, fn, default_or_policies, detail::policy_cons<>(), detail::is_policy_cons<DefaultOrPolicies>());
		}

		template<typename F, typename Default, typename... Policies>
		class_&& def(char const* name, const F fn, Default default_, const detail::policy_cons<Policies...> policies) &&
		{
			return std::move(*this).virtual_def(name, fn, default_, policies, std::false_type());
		}

		template<typename... Ts>
		class_&& def(constructor<Ts...> sig) &&
		{
            return std::move(*this).def_constructor(
                std::is_same<WrappedType, detail::null_type>(), sig, detail::policy_cons<>()
			);
		}

		template<typename... Policies, typename... Ts>
		class_&& def(constructor<Ts...> sig, const detail::policy_cons<Policies...> policies) &&
		{
            return std::move(*this).def_constructor(
				std::is_same<WrappedType, detail::null_type>(), sig, policies
			);
		}

		template<typename Getter>
		class_&& property(const char* name, Getter g) &&
		{
            using namespace std::placeholders;
			add_getter(name, std::bind(detail::get_caller<T, Getter>(), _1, _2, g));
            return std::move(*this);
		}

		template<typename Getter, typename MaybeSetter>
		class_&& property(const char* name, Getter g, MaybeSetter s) &&
		{
			return std::move(*this).property_impl(name, g, s, detail::is_policy_cons<MaybeSetter>());
		}

		template<typename Getter, typename Setter, typename... GetPolicies>
		class_&& property(const char* name, Getter g, Setter s, const detail::policy_cons<GetPolicies...>) &&
		{
            using namespace std::placeholders;

			add_getter(name, std::bind(detail::get_caller<T, Getter, GetPolicies...>(), _1, _2, g));
#ifndef LUABIND_NO_ERROR_CHECKING
			add_setter(
				name
				, std::bind(detail::set_caller<T, Setter>(), _1, _2, s)
				, detail::gen_set_matcher(static_cast<Setter>(nullptr))
				, &detail::get_member_signature<Setter>::apply);
#else
			add_setter(name, std::bind(detail::set_caller<T, Setter>(), _1, _2, s));
#endif
			return std::move(*this);
		}

		template<typename Getter, typename Setter, typename... GetPolicies, typename... SetPolicies>
		class_&& property(const char* name, Getter g, Setter s,
                          const detail::policy_cons<GetPolicies...>,
                          const detail::policy_cons<SetPolicies...> set_policies) &&
		{
            using namespace std::placeholders;

			add_getter(name, std::bind(detail::get_caller<T, Getter, GetPolicies...>(), _1, _2, g));
#ifndef LUABIND_NO_ERROR_CHECKING
			add_setter(
				name
				, std::bind(detail::set_caller<T, Setter, SetPolicies...>(), _1, _2, s)
				, detail::gen_set_matcher(static_cast<Setter>(nullptr), set_policies)
				, &detail::get_member_signature<Setter>::apply);
#else
			add_setter(name, std::bind(detail::set_caller<T, Setter, SetPolicies...>(), _1, _2, s));
#endif
			return std::move(*this);
		}

		template<typename D>
		class_&& def_readonly(const char* name, D T::*member_ptr) &&
		{
            using namespace std::placeholders;
			add_getter(name, std::bind(detail::auto_get<T,D>(), _1, _2, member_ptr));
			return std::move(*this);
		}

		template<typename D, typename... Policies>
		class_&& def_readonly(const char* name, D T::*member_ptr, const detail::policy_cons<Policies...>) &&
		{
            using namespace std::placeholders;
			add_getter(name, std::bind(detail::auto_get<T, D, Policies...>(), _1, _2, member_ptr));
            return std::move(*this);
		}

		template<typename D>
		class_&& def_readwrite(const char* name, D T::*member_ptr) &&
		{
            using namespace std::placeholders;
			add_getter(name, std::bind(detail::auto_get<T, D>(), _1, _2, member_ptr));
#ifndef LUABIND_NO_ERROR_CHECKING
			add_setter(
				name
				, std::bind(detail::auto_set<T, D>(), _1, _2, member_ptr)
				, &detail::set_matcher<D>::apply
				, &detail::get_setter_signature<D>::apply);
#else
			add_setter(name, std::bind(detail::auto_set<T, D>(), _1, _2, member_ptr));
#endif
            return std::move(*this);
		}

		template<typename D, typename... GetPolicies>
		class_&& def_readwrite(const char* name, D T::*member_ptr, const detail::policy_cons<GetPolicies...>) &&
		{
            using namespace std::placeholders;
			add_getter(name, std::bind(detail::auto_get<T, D, GetPolicies...>(), _1, _2, member_ptr));
#ifndef LUABIND_NO_ERROR_CHECKING
			add_setter(
				name
				, std::bind(detail::auto_set<T, D>(), _1, _2, member_ptr)
				, &detail::set_matcher<D>::apply
				, &detail::get_setter_signature<D>::apply);
#else
			add_setter(name, std::bind(detail::auto_set<T, D>(), _1, _2, member_ptr));
#endif
            return std::move(*this);
		}

		template<typename D, typename... GetPolicies, typename... SetPolicies>
		class_&& def_readwrite(const char* name, D T::*member_ptr, const detail::policy_cons<GetPolicies...>, const detail::policy_cons<SetPolicies...>) &&
		{
            using namespace std::placeholders;
			add_getter(name, std::bind(detail::auto_get<T, D, GetPolicies...>(), _1, _2, member_ptr));
#ifndef LUABIND_NO_ERROR_CHECKING
			add_setter(
				name
				, std::bind(detail::auto_set<T, D, SetPolicies...>(), _1, _2, member_ptr)
				, &detail::set_matcher<D, SetPolicies...>::apply
				, &detail::get_setter_signature<D>::apply);
#else
			add_setter(name, std::bind(detail::auto_set<T, D, SetPolicies...>(), _1, _2, member_ptr));
#endif
            return std::move(*this);
		}

		template<typename Derived, typename... Policies>
		class_&& def(detail::operator_<Derived>, const detail::policy_cons<Policies...> policies) &&
		{
			return std::move(*this).def(
				Derived::name()
			  , &Derived::template apply<T, Policies...>::execute
			  , raw<1>() + policies
			);
		}

		template<typename Derived>
		class_&& def(detail::operator_<Derived>) &&
		{
			return std::move(*this).def(
				Derived::name()
			  , &Derived::template apply<T>::execute
			  , raw<1>()
			);
		}

		detail::enum_maker<self_t> enum_(const char*) &&
		{
			return detail::enum_maker<self_t>(std::move(*this));
		}
		
		detail::static_scope<self_t> scope() &&
		{
            return detail::static_scope<self_t>(std::move(*this));
		}
		
	private:
        struct BasesPredicate
        {
            template <typename U>
            using type = std::disjunction<
                detail::is_bases<U>,
                std::is_base_of<U, T>
            >;
        };

		void init()
		{
			set_back_reference(static_cast<back_reference<T>*>(nullptr));

            using bases_t = typename detail::extract_parameter<
                BasesPredicate,
                no_bases,
                Xs...
            >::type;

            using Base = std::conditional_t<
                detail::is_bases<bases_t>::value,
                bases_t,
                bases<bases_t>
            >;
	
			class_base::init(LUABIND_TYPEID(T)
				, detail::internal_holder_type<HeldType>::apply()
				, detail::pointee_typeid(
					get_const_holder(static_cast<HeldType*>(nullptr)))
				, detail::internal_holder_extractor<HeldType>::apply(detail::type<T>())
				, detail::internal_const_holder_extractor<HeldType>::apply(detail::type<T>())
				, detail::const_converter<HeldType>::apply(
					luabind::get_const_holder(static_cast<HeldType*>(nullptr)))
				, detail::holder_constructor<HeldType>::apply(detail::type<T>())
				, detail::const_holder_constructor<HeldType>::apply(detail::type<T>())
				, detail::holder_default_constructor<HeldType>::apply(detail::type<T>())
				, detail::const_holder_default_constructor<HeldType>::apply(detail::type<T>())
				, get_adopt_fun(static_cast<WrappedType*>(nullptr)) // adopt fun
				, detail::internal_holder_destructor<HeldType>::apply(detail::type<T>())
				, detail::internal_const_holder_destructor<HeldType>::apply(detail::type<T>())
				, detail::internal_holder_size<HeldType>::apply()
				, detail::get_holder_alignment<HeldType>::apply());

			generate_baseclass_list(detail::type<Base>());
		}

		template<typename Getter, typename... GetPolicies>
		class_&& property_impl(const char* name,
									 Getter g,
									 const detail::policy_cons<GetPolicies...>,
									 std::true_type /*is policy_cons*/) &&
		{
            using namespace std::placeholders;
			add_getter(name, std::bind(detail::get_caller<T, Getter, GetPolicies...>(), _1, _2, g));
			return std::move(*this);
		}

		template<typename Getter, typename Setter>
		class_&& property_impl(const char* name,
									 Getter g,
									 Setter s,
									 std::false_type /*is policy_cons*/) &&
		{
            using namespace std::placeholders;

			add_getter(name, std::bind(detail::get_caller<T, Getter>(), _1, _2, g));
#ifndef LUABIND_NO_ERROR_CHECKING
			add_setter(
				name
				, std::bind(detail::set_caller<T, Setter>(), _1, _2, s)
				, detail::gen_set_matcher(static_cast<Setter>(nullptr), detail::policy_cons<>())
				, &detail::get_member_signature<Setter>::apply);
#else
			add_setter(name, std::bind(detail::set_caller<T, Setter>(), _1, _2, s));
#endif
			return std::move(*this);
		}

		// these handle default implementation of virtual functions
		template<typename F, typename... Policies>
		class_&& virtual_def(char const* name, const F fn, const detail::policy_cons<Policies...> policies, const detail::policy_cons<>, std::true_type /*is policy_cons*/) &&
		{
			// normal def() call

			if constexpr (!std::is_function_v<std::remove_pointer_t<F>> && !std::is_member_function_pointer_v<F>)
			{
				constexpr auto lambda_cast = cdecl_cast(fn, &F::operator());

				detail::overload_rep o(lambda_cast, policies);

				o.set_match_fun(detail::mem_fn_matcher<decltype(lambda_cast), T, Policies...>(lambda_cast));
				o.set_fun(detail::mem_fn_callback<decltype(lambda_cast), T, Policies...>(lambda_cast));

#ifndef LUABIND_NO_ERROR_CHECKING
				o.set_sig_fun(&detail::get_member_signature<decltype(lambda_cast)>::apply);
#endif
				this->add_method(name, std::move(o));
			}
			else
			{
				detail::overload_rep o(fn, policies);

				o.set_match_fun(detail::mem_fn_matcher<F, T, Policies...>(fn));
				o.set_fun(detail::mem_fn_callback<F, T, Policies...>(fn));

#ifndef LUABIND_NO_ERROR_CHECKING
				o.set_sig_fun(&detail::get_member_signature<F>::apply);
#endif
				this->add_method(name, std::move(o));
			}

			return std::move(*this);
		}

		template<typename F, typename Default, typename... Policies>
		class_&& virtual_def(char const* name, const F fn, Default const& default_, const detail::policy_cons<Policies...> policies, std::false_type /*is policy_cons*/) &&
		{
			// default_ is a default implementation
			// policies is either null_type or a policy list

			// normal def() call

			if constexpr (!std::is_function_v<std::remove_pointer_t<F>> && !std::is_member_function_pointer_v<F>)
			{
				constexpr auto lambda_cast = cdecl_cast(fn, &F::operator());

				detail::overload_rep o(lambda_cast, policies);

				o.set_match_fun(detail::mem_fn_matcher<decltype(lambda_cast), T, Policies...>(lambda_cast));
				o.set_fun(detail::mem_fn_callback<decltype(lambda_cast), T, Policies...>(lambda_cast));

				o.set_fun_static(detail::mem_fn_callback<Default, T, Policies...>(default_));

#ifndef LUABIND_NO_ERROR_CHECKING
				o.set_sig_fun(&detail::get_member_signature<decltype(lambda_cast)>::apply);
#endif

				this->add_method(name, std::move(o));
			}
			else
			{
				detail::overload_rep o(fn, policies);

				o.set_match_fun(detail::mem_fn_matcher<F, T, Policies...>(fn));
				o.set_fun(detail::mem_fn_callback<F, T, Policies...>(fn));

				o.set_fun_static(detail::mem_fn_callback<Default, T, Policies...>(default_));

#ifndef LUABIND_NO_ERROR_CHECKING
				o.set_sig_fun(&detail::get_member_signature<F>::apply);
#endif

				this->add_method(name, std::move(o));
			}

			// register virtual function
			return std::move(*this);
		}

        template<typename... Policies, typename... Ts>
		class_&& def_constructor(
			std::true_type /* HasWrapper */
          , constructor<Ts...>
          , detail::policy_cons<Policies...>) &&
        {	
			detail::construct_rep::overload_t o;

			o.set_constructor(&detail::construct_class<T, Policies...>::template apply<Ts...>);
			o.set_match_fun(&detail::constructor_match<2, Ts...>::template apply<Policies...>);

#ifndef LUABIND_NO_ERROR_CHECKING
			o.set_sig_fun(&detail::get_signature<Ts...>::apply);
#endif
			o.set_arity(detail::calc_arity<sizeof...(Ts)>::template apply<Policies...>());
			this->add_constructor(std::move(o));
            return std::move(*this);
        }

        template<typename... Policies, typename... Ts>
		class_&& def_constructor(
			std::false_type /* !HasWrapper */
          , constructor<Ts...>
          , detail::policy_cons<Policies...>) &&
		{
			detail::construct_rep::overload_t o;

			o.set_constructor(&detail::construct_wrapped_class<T, WrappedType, Policies...>::template apply<Ts...>);
			o.set_match_fun(&detail::constructor_match<2, Ts...>::template apply<Ts...>);

#ifndef LUABIND_NO_ERROR_CHECKING
			o.set_sig_fun(&detail::get_signature<Ts...>::apply);
#endif
			o.set_arity(detail::calc_arity<sizeof...(Ts)>::template apply<Policies...>());
			this->add_constructor(std::move(o));
            return std::move(*this);
        }

		void set_back_reference(detail::default_back_reference*)
		{
			back_reference<T>::has_wrapper = !std::is_same_v<WrappedType, detail::null_type>;
		}

		void set_back_reference(void*) const
		{
		}

		typedef void(*adopt_fun_t)(void*);

		template<typename W>
		adopt_fun_t get_adopt_fun(W*)
		{
            return &detail::adopt_function<T, W>::execute;
		}

		adopt_fun_t get_adopt_fun(detail::null_type*)
		{
			return nullptr;
		}
	};

	namespace 
	{
		detail::policy_cons<detail::pure_virtual_tag> pure_virtual;
	}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
