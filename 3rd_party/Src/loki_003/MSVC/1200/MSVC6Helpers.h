// Last update: Mar 06, 2003
// Added VoidWrap
// Added qualification ::Loki::Private:: to types from the Private-Namespace
// Thanks to Adi Shavit

#ifndef MSVC6HELPERS__H
#define MSVC6HELPERS__H
#if !defined (_MSC_VER) || _MSC_VER >= 1300
#error "please use this header only with MSVC 6.0"
#endif
#include "TypeManip.h"
namespace Loki
{
	namespace Private
	{
		template <class T, class U>
		struct VC_InaccessibleBase : public T
		{};

		// workaround for the "Error C2516. : is not a legal base class"
		// Use VC_Base_Workaround's LeftBase instead of the
		// alleged illegal base class.
		template <class T, class U>
		struct VC_Base_Workaround : public T, public U
		{
			typedef T LeftBase;
		};

		// MSVC 6.0 does not allow the return of
		// expressions of type "cv void" in a functions with a return
		// type of cv void (6.6.3).
		// Functor.h uses this Type as a workaround.
		struct VoidAsType {};
		
		// workaround for error C2182: '__formal' illegal use of type 'void'
		// when trying to use void as default value of a template parameter.
		// Instead of template<class T = void> class bla {};
		// simply write template <class T = VoidWrap::type> class bla {};
		// and the VC 6.0 will be happy.
		struct VoidWrap
		{
			typedef void type;
		};
		// workarounds for template template parameters
////////////////////////////////////////////////////////////////////////////////
// class template AlwaysFalse
// Invocation: AlwaysFalse<T>::value
// value will always by 0 (false)
////////////////////////////////////////////////////////////////////////////////
		template< typename T >
		struct AlwaysFalse
		{
			enum { value = false };
		};

////////////////////////////////////////////////////////////////////////////////
// class template ApplyImpl1
// Invocation: ApplyImpl1<T>::template Result<T1>
// T must be a nontemplate type with a nested class template named In.
// The class template is a helper for the Apply1-Template
////////////////////////////////////////////////////////////////////////////////
		template <class TypeWithNestedTemplate>
		struct ApplyImpl1
		{
			template<bool flag>
			struct VC_WORKAROUND : public TypeWithNestedTemplate {};

			struct VC_WORKAROUND<true>
			{	template<class> struct In; };

			template< typename T1 > struct Result : public
			VC_WORKAROUND< ::Loki::Private::AlwaysFalse<TypeWithNestedTemplate>::value >::template In<T1>
			{
				typedef VC_WORKAROUND< ::Loki::Private::AlwaysFalse<TypeWithNestedTemplate>::value >::template In<T1> Base;

			};
		};
////////////////////////////////////////////////////////////////////////////////
// class template ApplyImpl2
// Invocation: ApplyImpl2<T>::template Result<T1, T2>
// T must be a nontemplate type with a nested class template named In.
// The class template is a helper for the Apply2-Template
////////////////////////////////////////////////////////////////////////////////
		template <class TypeWithNestedTemplate>
		struct ApplyImpl2
		{
			template<bool flag>
			struct VC_WORKAROUND : public TypeWithNestedTemplate {};

			struct VC_WORKAROUND<true>
			{template<class T, class U> struct In; };

			template< typename T1, typename T2 > struct Result : public
			VC_WORKAROUND< ::Loki::Private::AlwaysFalse<TypeWithNestedTemplate>::value >::template In<T1, T2>
			{
			};
		};

	}	// end of namespace Private


////////////////////////////////////////////////////////////////////////////////
// class template Apply1
// Invocation: Apply1<T, U>
// Applies the type U to the inner template In of type T
// The class template Apply1 helps to emulate template template parameters
// i first saw this technique in boost's mpl library.
////////////////////////////////////////////////////////////////////////////////
	template<typename F, typename T1>
	struct Apply1 : ::Loki::Private::ApplyImpl1<F>::template Result<T1>
	{
		typedef typename ::Loki::Private::ApplyImpl1<F>::template Result<T1>::Base Base;
	};
////////////////////////////////////////////////////////////////////////////////
// class template Apply2
// Invocation: Apply2<T, U, V>
// Applies the types U and V to the inner template In of type T
// The class template Apply2 helps to emulate template template parameters
// i first saw this technique in boost's mpl library.
////////////////////////////////////////////////////////////////////////////////
	template<typename F, typename T1, typename T2>
	struct Apply2 : ::Loki::Private::ApplyImpl2<F>::template Result<T1, T2>
	{

	};

////////////////////////////////////////////////////////////////////////////////
// class template ApplyInnerType
// Invocation: ApplyInnerType<Wrapper, U>::type
// Applies the type U to the the inner template 'In' of type Wrapper and typedefs
// the resulting type to 'type'
// The class template ApplyInnerType helps to emulate template template parameters
// i first saw this technique in boost's mpl library.
////////////////////////////////////////////////////////////////////////////////
	template <class Wrapper, class T>
	struct ApplyInnerType
	{
		template<bool> struct Wrapper_VC : Wrapper {};

		template<> struct Wrapper_VC<true>
		{ template<class X> struct In; };
		typedef typename
		Wrapper_VC< ::Loki::Private::AlwaysFalse<Wrapper>::value>::template In<T>::type type;
	};
////////////////////////////////////////////////////////////////////////////////
// class template ApplyInnerType2
// Invocation: ApplyInnerType2<Wrapper, U, V>::type
// Applies the types U and V to the the inner template 'In' of type Wrapper and typedefs
// the resulting type to 'type'
// The class template ApplyInnerType helps to emulate template template parameters
// i first saw this technique in boost's mpl library.
////////////////////////////////////////////////////////////////////////////////
	template <class Wrapper, class T, class V>
	struct ApplyInnerType2
	{
		template<bool> struct Wrapper_VC : Wrapper {};

		template<> struct Wrapper_VC<true>
		{ template<class X, class Y> struct In; };
		typedef typename
		Wrapper_VC< ::Loki::Private::AlwaysFalse<Wrapper>::value>::template In<T, V>::type type;
	};

	template <class Wrapper, class T, class U, class V, class W>
	struct ApplyInnerType4
	{
		template<bool> struct Wrapper_VC : Wrapper {};

		template<> struct Wrapper_VC<true>
		{ template<class W, class X, class Y, class Z> struct In; };
		typedef typename
		Wrapper_VC< ::Loki::Private::AlwaysFalse<Wrapper>::value>::template In<T, U, V, W>::type type;
	};


}
#endif