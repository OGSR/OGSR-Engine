Loki VC 6.0 Port or how to produce C1001 - Internal Compiler Errors
-------------------------------------------------------------------
Version: 0.5d

Introduction/Compatibility:
---------------------------
This is a partial MSVC 6.0 Sp5 compatible port of Andrei Alexandrescu's excellent Loki Library.
Because I could not retain the originial interface in all places, this port is not
compatible to the original library and therefore code using this port *cannot* generally be
used together with the original lib.
This is, of course, a great pity.
So if you know of a complete and full interface-compatible VC 6.0
port or if you know how to improve this port, please let me know.

Contact:
--------
For any suggestions, bug reports, comments and questions please email me to
Hume@c-plusplus.de

Using this port:
----------------
To use this port, simply extract the files from the archive, give your compiler access to
their path, and include them appropriately in your code via #include.

If you use the small object allocator directly or indirectly (through the Functor class)
you must add SmallObj.cpp to your project/makefile.

If you use Singletons with longevity you must add Singleton.cpp to your project/makefile.

Fixes:
------
    Mar 21, 2003:
    -------------
        * In MultiMethods.h: Added a new explicit template argument specification (ETAS)-workaround 
        for FnDispatcher::Add which is more compliant with other 
        ETAS-workarounds used in this port.

    Mar 20, 2003:
    -------------
        * In MultiMethods.h: Fixed bugs in FnDispatcher and FunctorDispatcher.
        Fixing FnDispatcher led to an Interface change (see section "Interface changes").

	Mar 08, 2003:
    -------------
        * In HierarchyGenerators.h: implemented transparent workaround for
        'explicit template argument specification for nonmeber functions'-bug.
        The Field-Functions can now be called as in the original lib.

    Mar 06, 2003:
    -------------
        * In SmartPointer.h: Added helper-macros for convenient specialization
        of std::less for Smart-Pointers.

        * I found a way to use void as a default value for template parameters.
        Therefore I changed MultiMethods.h and Visitor.h accordingly.


    Feb 2003:
	---------
		* created new versions of Functor.h, Visitor.h and MultiMethods.h that
		now can handle void return types transparently.

		* ported SmartPtr's Ownership-Policy RefCountedMT

		* Added isFunctionPointer to TypeTraits.

		* Replaced all pointer-type dummy-parameters needed as a workaround
		for VC's 'explicit template argument specification'-bug with Typ2Type-dummy
		parameters.

		* fixed the problems with BindFirst (Functor.h) that led to
		C1001-Internal compiler errors.

		* fixed numerous other bugs.


    Jan 30, 2003:
    -------------
        * In TypeTraits.h: Fixed bugs in TypeTraits' scalar and array detection.
        const and volatile detection is now based on techniques from boost's type traits
        (see http://www.boost.org/libs/type_traits/)
        Added Enum- and pointer-to-member-function-detection code.
        Thanks to M. Yamada.


    Jan 12, 2003:
    -------------
    	* changed the signature of SmallObject's op new. Now it
    	matches the corresponding op delete.
		Thanks to M.Yamada for the hint and the solution.

    Dec 08, 2002:
    -------------
        * In HierarchyGenerators.h: Sergey Khachatrian reported a bug
        in GenScatterHierarchy when used with a typelist containing
        equal types (e.g. GenScatterHierarchy<TYPELIST_2(int, int), UnitWrapper>
        resp. Tuple<TYPELIST_2(int, int)>)
        Fixing the bug I found another MSVC6-Problem in the Field-function.
        The workaround for this problems results in an interface change.

        please refer to the section "Interface changes" below for further information.

    Dec 03, 2002
    -------------
        * In MSVC6Helpers.h: The original version failed to qualify some types from the
        Private-Namespace.
        Thanks to Adi Shavit for pointing that out

        * In Threads.h: Changed wrong ctor/dtor names in ObjectLevelLockable.
        Thanks to Adi Shavit for pointing that out

    Nov 19, 2002:
    -------------
        * In SmartPtr.h: Changed template ctors. See Notes.

Notes:
------
The original Loki Lib uses some pretty advanced (resp. new) C++ features like:

A. partial template specialization.
B. template template parameters.
C. explicit template argument specification for member- and nonmeber functions.
D. covariant return types.
E. Template parameters with default type void
F. return statements with an expression of type cv void in functions with a return type of cv void.

Unfortunately the MSVC 6.0 supports neither of them.

    A. I used various techniques to simulate partial template specialization. In some cases
    these techniques allowed me to retain the original interfaces but often that was not
    possible (or better: i did not find a proper solution). In any case it led
    to increasing code complexity :-)

    B. One way to simulate template template parameters is to replace the template class with
    a normal class containing a nested template class. You then move the original functionality
    to the nested class.
    The problem with this approach is MSVC's 'dependent template typedef bug'.
    MSVC 6.0 does not allow something like this:

    [code]
    template <class APolicy, class T>
    struct Foo
    {
        // error C2903: 'In' : symbol is neither a class template nor a function template
        typedef typename APolicy::template In<T> type;
    };
    [/code]

    To make a long story short, I finally decided to use boost::mpl's apply-technique to
    simulate template template parameters. This approach works fine with MSVC 6.0. But be warned,
    this technique uses not valid C++.
    Of course, replacing template template parameters always results in some interface changes.

    C. I added dummy-Parameters to (Member-)Functions that depend on explicit template
    argument specification. These dummy-Parameters help the compiler in deducing the template
    parameters that otherwise need to be explicitly specified.
    Example:
    [code]
    struct Foo
    {
        template <class T>
        T Func();
    };
    [/code]
    becomes
    [code]
    struct Foo
    {
        template <class T>
        T Func(Type2Type<T>);
    };
    [/code]
    in this port.

    Update:
    -------
    The MSVC 6.0 sometimes does not overload normal functions depending
    on explicit argument specification correctly (see: Microsoft KB Article - 240871)
    The following code demonstrates the problem:
    [code]
    template <unsigned i, class T>
    void BugDemonstration(T p)
    {
	    printf("BugDemonstration called with i = %d\n", i);
    }

    int main()
    {
        GenScatterHierarchy<TYPELIST_3(int, int, int), TestUnitWrapper> Bla;
	    // will always print: "BugDemonstration called with i = 2";
	    BugDemonstration<0>(Bla);
	    BugDemonstration<1>(Bla);
	    BugDemonstration<2>(Bla);
    }
    [/code]

    Fortunately there is a transparent workaround for this problem. Simply add
    a dummy-parameter with a proper default value:
    [code]
    template <unsigned i, class T>
    void BugDemonstration(T p, Int2Type<i>* = (Int2Type<i>*)0)
    {
	    printf("BugDemonstration called with i = %d\n", i);
    }

    int main()
    {
        GenScatterHierarchy<TYPELIST_3(int, int, int), TestUnitWrapper> Bla;
	    // will now work correctly
	    BugDemonstration<0>(Bla);
	    BugDemonstration<1>(Bla);
	    BugDemonstration<2>(Bla);
    }
    [/code]

    Unfortunately adding dummy-parameters does not always work.
    For example for one of FnDispatcher's Add-member-functions you have to explicitly
    specify two type- and one non-type parameter.
    [code]
    template </*...*/typename ResultType/*...*/>
    class FnDispatcher
    {
    public:
	    //...
	    template <class SomeLhs, class SomeRhs,
		    ResultType (*callback)(SomeLhs&, SomeRhs&)>
	    void Add(){/*...*/}
    };
    //...
    FnDispatcher<Shape> dis;
    dis.Add<Poly, Poly, &AFunc>();
    [/code]
    Using dummy-parameters as workaround FnDispatcher::Add would become something
    like this:
    [code]
    template<class S1, class S2, class R, R (*)(S1&,S2&)>
    struct Helper {};

    template </*...*/typename ResultType/*...*/>
    class FnDispatcher
    {
    public:
	    //...
	    template <class SomeLhs, class SomeRhs,
		    ResultType (*callback)(SomeLhs&, SomeRhs&)>
	    void Add(Helper<SomeLhs, SomeRhs, ResultType, callback>)
	    {}
    };
    //...
    FnDispatcher<void> f;
	f.Add(Helper<Rectangle, Rectangle, void, &Func>());
    [/code]
    This compiles fine, but alas Add never gets called. I don't know what happens,
    I only know that the MSVC 6.0 won't generate code for a function call.

    In situations like that, instead of dummy-Parameters I used nested template-classes
    with overloaded function-operator as a workaround.
    [code]
    template </*...*/typename ResultType/*...*/>
    class FnDispatcher
    {
    public:
	    // the member-function Add becomes a member-template-class
	    // with overloaded function operator.
	    template <class SomeLhs, class SomeRhs,
		    ResultType (*callback)(SomeLhs&, SomeRhs&)>
	    struct AddI
	    {
		    void operator()(FnDispatcher<ResultType>& o)  {/*...*/}
	    };
    };
    //...
    FnDispatcher<void> f;
	FnDispatcher<void>::AddI<Rectangle, Rectangle, &Func>()(f);
    [/code]

    If you know of a better workaround, please let me know.
    
    Update:
    -------
    The problem in the example above is Add's nontype-function-pointer-Parameter.
    If one changes this parameter to a type-parameter the problem vanishes.
    The example above then becomes:
    [code]
    template </*...*/typename ResultType/*...*/>
    class FnDispatcher
    {
    public:
	    // Etas stands for explicit template argument specification.
        // Do whatever you need to do with callback in this class.
        template <class SomeLhs, class SomeRhs, 
			ResultType (*callback)(SomeLhs&, SomeRhs&), bool symmetric = false>
		struct Etas
		{/*...*/};
		
		// EtasType has to be a template parameter. If one tries to use
		// a parameter of type Etas the MSVC 6.0 won't generate correct
		// code.
		template <class EtasType>
        void Add(EtasType EtasObj)
        {/*...*/}
    };
    //...
    typedef FnDispatcher<void> DisType;
    DisType f;
	f.Add(DisType::Etas<Rectangle, Rectangle, &Func>());
    [/code]

    The port provides both workarounds but the use of the second should be preferred,
    because it betters fits to the rest of the port's workarounds.

    D. Virtual functions that use covariant return types (e.g. return a pointer to Derived)
    in the original library were changed so that they have exactly the
    same return type as the original virtual function (e.g. return a pointer to Base).

    E. The MSVC 6.0 does not allow code like this:
    [code]
    // error C2182: '__formal' illegal use of type 'void'
    template <class T, class R = void>
    struct Blub {};
    [/code]

    Interestingly enough you can have void as default type by simply using another
    level of indirection:
    [code]
    struct VoidWrap
    {
	    typedef void type;
    };

    template <class T, class R = VoidWrap::type>
    struct Blub
    {};
    [/code]

    F. To workaround void returns I did the following:
    From every original class I moved those functions that potentially
	produce void returns to new classes. One for the general case and
	one for the void case.
	In the class for the general case I implemented the functions in the original way.
	In the class for the void case I removed the return statements and therefore the
	potential void return.
	Depending on the return type, the original class inherits from the
	corresponding new class and thus gets the proper implementation of
	the previously removed functions.

	For example:
	[code]
	template <class R> struct Foo
	{
		R Func() { return R(); }
	};
	[/code]
	becomes:
	[code]
	namespace Private
	{
		template <class R> struct FooBase
		{
			R Func() {return R();}
		};
		struct FooVoidBase
		{
			typedef void R;
			R Func() {}
		};
	}
	template <class R>
	struct Foo	: public Select<IsVoid<R>::value, FooVoidBase, FooBase<R> >::Result
	{};
	[/code]
	
    The MSVC 6 allows explicit template specialization in class scope.
    In contrast the C++ Standards only allows explicit template specialization
    in namespace scope. Using the non-compliant feature, the implementation
    of the example above becomes a little less complicated:
    [code]
    namespace Private
    {
	    struct FooBase
	    {
		    template <class R> 
		    struct In
		    {
			    R Func() {return R();}
		    };
		    template <> 
		    struct In<void>
		    {;
			    void Func() {}    
		    };
	    };
    }
    template <class R>
    struct Foo	: Private::FooBase::In<R>
    {};
    [/code]
    
    Please note that *all* new base classes are only meant as a hidden
	implementation detail.
	You should never use any of them directly or indirectly. In particular don't
	make use of the possible derived-to-base conversion.

    In the old version of Functor.h I changed a ResultType of type void to
    VoidAsType (an udt). This change is transparent to the user of Functor.

Some words to template-ctors resp. template assignment operators:
The MSVC 6.0 introduces an order-dependency for template ctor
resp. template assignemt operators.
If you need both a copy-ctor and a template copy ctor (same for copy-assignment), then
you *must* write the templated version first.
So instead of
[code]
template <class T>
struct Foo
{
    Foo(const Foo&)
    {}
    template <class U>
    Foo(const Foo<U>& r)
    {}
};
[/code]
you *need* to write:
[code]
template <class T>
struct Foo
{
    template <class U>
    Foo(const Foo<U>& r)
    {}

    Foo(const Foo& r)
    {}
};
[/code]

Many thanks to Nelson Elói for pointing that out and for providing me
with this solution.

The above solution unfortunately does not work if the template ctor does not have
the form of a copy-ctor. If you write something like this (as in the functor-class):
[code]
template <class T>
struct Foo
{
    template <class Fun>
    Foo(Fun r)
    {}

    Foo(const Foo& r)
    {}
};
[/code]
then the VC will no longer find a copy-ctor.

Because of this, i can't use Nelson Elói's solution in Functor.h

Interface changes:
------------------
1. In Threads.h:

    *   Thread-Policies changed from class templates to normal classes containing a
    nested class template 'In'.

    consequences:
    This change is not very dramatic because it won't break code using this port when
    switching to the original library (only new Thread-Policies must be changed)

2. In Singleton.h:

    *   The Creation- and Lifetime-Policies are no longer class templates. Instead they all use
    Member-Templates.

    consequences:
    Again this change will only break new Policies when switching to the
    original library.

3. In Functor.h:

    *   No covariant return types.

    consequences:
    DoClone always returns a FunctorImplBase<R, ThreadingModel>* where R is the functor's return
    type and  ThreadingModel its current ThreadingModel.

4. TypeTraits.h

    *   Because VC 6.0 lacks partial template specialization, the TypeTraits-Class
    fails to provide the following typedefs:
    PointeeType, ReferredType, NonVolatileType and UnqualifiedType.

    *   Since the VC 6 does not differentiate
    between void, const void, volatile void and const volatile void the following
    assertions will fail:
    assert(TypeTraits<const void>::isConst == 1)
    assert(TypeTraits<volatile void>::isVolatile == 1)
    assert(TypeTraits<const volatile void>::isConst == 1)
    assert(TypeTraits<const volatile void>::isVolatile == 1)

    *   This port adds isEnum, isMemberFunctionPointer and isFunctionPointer.


5. HierarchyGenerator.h

    *   I used Mat Marcus' approach to port GenScatterHierarchy.
    See http://lists.boost.org/MailArchives/boost/msg20915.php) for the consequences.

    *   Same for GenLinearHierarchy

    *   Unit is no longer a template template parameter.

    consequences:
    For every concrete unit-template there must be a normal class containing
    a nested-template class called 'In'. 'In' should only contain a typedef to the
    concrete Unit.

    Update:
    The port's original version of GenScatterHierarchy does not work when used
    with typelists containing equal types.
    The problem is due to a VC bug. The VC fails to compile code similar
    to this, although it is perfectly legal.
    [code]
    template <class T>
    class Wrapper
    {};

    template <class T>
    struct B : public Wrapper<T>
    {};

    // ERROR: 'A<T>' : direct base 'Wrapper<T>' is inaccessible; already a base of 'B<T>'
    template <class T>
    class A : public B<T>, public Wrapper<T>
    {};
    [/code]

    Unfortunately my workaround has a big drawback.
    GenScatterHierarchy now has to generate a lot more classes.
    Alexandrescu's original implementation generates 3*n classes (n - number of types in the typelist)
    The old version of my port creates 4 * n + 1
    The new version will create 5 * n

    The fix also reveals the "Explicitly Specified Template Functions Not Overloaded Correctly"-Bug
    (Microsoft KB Article - 240871) in the Field-Function taking a nontype int Parameter.
    See Notes (section C) for the description of the workaround.

    I also added a macro FIELD. Using this macro one can write
    FIELD(obj, 0)


6. Factory.h

    *   The Error-Policy for Factory and CloneFactory is no longer a template template parameter.
    Use a class with member-templates instead.

    consequences:
    This change will only break new Policies when switching to the
    original library.

7. AbstractFactory.h

    *   no covariant return types

    *   no template template parameters
    For every concrete Factory-Unit there must be a normal class containing
    a nested-template class called 'In'. 'In' shall contain a typedef to the
    concrete Factory-Unit.

    *   Added a dummy-Parameter to AbstractFactory::Create (see C.)
    Calling syntax changed from:
        ConcProduct* p = aFactory.Create<ConcProduct>();
    to
        ConcProduct* p = aFactory.Create(Type2Type<ConcProduct>());


8. SmartPtr.h

   * no template template parameters.
   (see 7.for a description of the consequences)

   * This port does not specialize std::less

   Update:
   -------
   The port provides some helper-macros for convenient specialization
   of std::less for Smart-Pointers.

   If, for example, you want to use a Smart-Pointer as the key of a std::map,
   you can do it like this:
   [code]
   #include <map>
   #include <loki/SmartPtr.h>

   SMARTPTR_SPECIALIZE_LESS(Apple)

   class Apple {};

    int main()
    {
        std::map<SmartPointer<Apple>, int> m;
        //...
    }
    [/code]

9. Visitor.h

    * no template template parameters
    (see 7.for a description of the consequences)

    * This port fails to correctly support void return types. As a workaround it provides
    a set of complete new classes (and macros) for void. Default arguments of type void
    were replaced by arguments of type int.

	Update:
	-------
	In the new version of Visitor.h there are no longer extra classes for void.
	Instead the original classes are now able to handle the return type void.
	However there are still two sets of macros. One for return type = void
	(DEFINE_VISITABLE_VOID, DEFINE_CYCLIC_VISITABLE_VOID) and one for return
	type != void (DEFINE_VISITABLE, DEFINE_CYCLIC_VISITABLE)


10. MultiMethods.h

    * replaced all template template parameters with 'normal' parameters (see 7.
    for a description of the consequences)

    * This port does not support functions with return type void.

    * dummy parameters were added to functions that otherwise would depend on
    explicit template argument specification (14.8.1).

	Update:
	-------
	* The port now supports functions with return type void.

	Some words to BasicDispatcher:
	------------------------------
	You can't use a (namespace level) template function as callback-function
	for BasicDispatcher. This is because using the VC 6.0 you can't explicity
	specify the template-paramters when adding the concrete function instance
	to the dispatcher.
	Normaly you can write something like this:
	[code]
	template <class DerivedShape1, class DerivedShape2>
	int HatchShapes(Shape&, Shape&) {...}

	typedef ::Loki::BasicDispatcher<Shape> Dispatcher;

	void Func(Dispatcher& x)
	{
		x.Add(&HatchShapes<Circle, Rectangle>);
	}
	[/code]
	Using the VC 6.0 this is not possible, because there is no
	way to specify the types for DerivedShape1 and DerivedShape2 (at least
	I know of no way).

	As a workaround use a helper-template class in conjunction with
	a static member function:
	[code]
	template <class DerivedShape1, class DerivedShape2>
	struct Hatch_Helper
	{
		int HatchShapes(Shape&, Shape&) {...}
	};

	typedef ::Loki::BasicDispatcher<Shape> Dispatcher;

	void Func(Dispatcher& x)
	{
		x.Add(&Hatch_Helper<Circle, Rectangle>::HatchShapes);
	}
    [/code]
    Some words to FnDispatcher:
	---------------------------
    The trampoline-Versions of FnDispatcher::Add differ
    from the original library.

    Using the original library one writes:
    [code]
    typedef FnDispatcher<Shape> Dispatcher;
    void Hatch(Rectangle& lhs, Poly& rhs) {...}

    Dispatcher dis;
    disp.Add<Rectangle, Poly, &Hatch>();
    [/code]
    
    Using this port the last line either becomes:
    [code]
    disp.Add(Dispatcher::Etas<Rectangle, Poly, &Hatch>());
    [/code]
    or
    [code]
    Dispatcher::AddI<Rectangle, Poly, &Hatch>()(dis);
    [/code]

More info:
----------
The original Loki library can be found here: http://moderncppdesign.com
For Rani Sharoni's VC 7.0 port see: http://www.geocities.com/rani_sharoni/LokiPort.html
        