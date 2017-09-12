'''
Module declarations

    Defines classes that represent declarations found in C++ header files.
    
'''

class Declaration(object):
    'Represents a basic declaration.'

    def __init__(self, name, namespace):
        # the declaration name
        self.name = name
        # all the namespaces, separated by '::' = 'boost::inner'
        self.namespace = namespace
        # tuple (filename, line)
        self.location = '', -1


    def FullName(self):
        'Returns the full qualified name: "boost::inner::Test"'
        namespace = self.namespace or ''
        #if not namespace:
        #    namespace = ''
        if namespace and not namespace.endswith('::'):
            namespace += '::'
        return namespace + self.name
    
    
    def __repr__(self):        
        return '<Declaration %s at %s>' % (self.FullName(), id(self))


    def __str__(self):
        return 'Declaration of %s' % self.FullName()
    


class Class(Declaration):
    'The declaration of a class or struct.'
    
    def __init__(self, name, namespace, members, abstract, bases):
        Declaration.__init__(self, name, namespace)
        # list of members
        self.members = members
        # whatever the class has any abstract methods
        self.abstract = abstract
        # instances of Base
        self.bases = bases
        self._members_count = {}


    def __iter__(self):
        return iter(self.members)            


    def IsAbstract(self):
        'Returns True if any method of this class is abstract'
        for member in self.members:
            if isinstance(member, Method):
                if member.abstract:
                    return True
        return False

    
    def RawName(self):
        'Returns the raw name of a template class. name = Foo<int>, raw = Foo'
        lesspos = self.name.find('<')
        if lesspos != -1:
            return self.name[:lesspos]
        else:
            return self.name        


    def Constructors(self, publics_only=True):
        constructors = []
        for member in self:
            if isinstance(member, Constructor):
                if publics_only and member.visibility != Scope.public:
                    continue
                constructors.append(member)
        return constructors

    
    def HasCopyConstructor(self):
        for cons in self.Constructors():
            if cons.IsCopy():
                return True
        return False


    def HasDefaultConstructor(self):
        for cons in self.Constructors():
            if cons.IsDefault():
                return True
        return False


    def IsUnique(self, member_name):
        if not self._members_count:
            for m in self:
                self._members_count[m.name] = self._members_count.get(m.name, 0) + 1
        try:
            return self._members_count[member_name] == 1
        except KeyError:
            print self._members_count
            print 'Key', member_name



class NestedClass(Class):
    'The declaration of a class/struct inside another class/struct.'

    def __init__(self, name, class_, visib, members, abstract, bases):
        Class.__init__(self, name, None, members, abstract, bases)
        self.class_ = class_
        self.visibility = visib


    def FullName(self):
        return '%s::%s' % (self.class_, self.name)
    

    
class Base:
    'Represents a base class of another class.'

    def __init__(self, name, visibility=None):
        # class_ is the full name of the base class
        self.name = name
        # visibility of the derivation
        if visibility is None:
            visibility = Scope.public
        self.visibility = visibility



class Scope:
    public = 'public'
    private = 'private'
    protected = 'protected'


    
class Function(Declaration):
    'The declaration of a function.'
    
    def __init__(self, name, namespace, result, params):
        Declaration.__init__(self, name, namespace)
        # the result type: instance of Type, or None (constructors)            
        self.result = result
        # the parameters: instances of Type
        self.parameters = params


    def PointerDeclaration(self):
        'returns a declaration of a pointer to this function'
        result = self.result.FullName()
        params = ', '.join([x.FullName() for x in self.parameters]) 
        return '(%s (*)(%s))&%s' % (result, params, self.FullName())

    
    def _MinArgs(self):
        min = 0
        for arg in self.parameters:
            if arg.default is None:
                min += 1
        return min

    minArgs = property(_MinArgs)
    

    def _MaxArgs(self):
        return len(self.parameters)

    maxArgs = property(_MaxArgs)
    
   
   
class Operator(Function):
    'The declaration of a custom operator.'
    def FullName(self):
        namespace = self.namespace or ''
        if not namespace.endswith('::'):
            namespace += '::'
        return namespace + 'operator' + self.name 



class Method(Function):
    'The declaration of a method.'

    def __init__(self, name, class_, result, params, visib, virtual, abstract, static, const):
        Function.__init__(self, name, None, result, params)
        self.visibility = visib
        self.virtual = virtual
        self.abstract = abstract
        self.static = static
        self.class_ = class_
        self.const = const

    
    def FullName(self):
        return self.class_ + '::' + self.name


    def PointerDeclaration(self):
        'returns a declaration of a pointer to this function'
        result = self.result.FullName()
        params = ', '.join([x.FullName() for x in self.parameters]) 
        const = ''
        if self.const:
            const = 'const'            
        return '(%s (%s::*)(%s) %s)&%s' %\
            (result, self.class_, params, const, self.FullName()) 

    
class Constructor(Method):
    'A constructor of a class.'

    def __init__(self, name, class_, params, visib):
        Method.__init__(self, name, class_, None, params, visib, False, False, False, False)


    def IsDefault(self):
        return len(self.parameters) == 0


    def IsCopy(self):
        if len(self.parameters) != 1:
            return False
        param = self.parameters[0]
        class_as_param = self.parameters[0].name == self.class_
        param_reference = isinstance(param, ReferenceType) 
        return param_reference and class_as_param and param.const
        

class Destructor(Method):
    'The destructor of a class.'

    def __init__(self, name, class_, visib, virtual):
        Method.__init__(self, name, class_, None, [], visib, virtual, False, False, False)

    def FullName(self):
        return self.class_ + '::~' + self.name



class ClassOperator(Method):
    'The declaration of a custom operator in a class.'
    
    def FullName(self):
        return self.class_ + '::operator ' + self.name


    
class ConverterOperator(ClassOperator):
    'An operator in the form "operator OtherClass()".'
    
    def FullName(self):
        return self.class_ + '::operator ' + self.result.name

    

class Type(Declaration):
    'Represents a type.'

    def __init__(self, name, const=False, default=None):
        Declaration.__init__(self, name, None)
        # whatever the type is constant or not
        self.const = const
        # used when the Type is a function argument
        self.default = default
        self.volatile = False

    def __repr__(self):
        if self.const:
            const = 'const '
        else:
            const = ''
        return '<Type ' + const + self.name + '>'


    def FullName(self):
        if self.const:
            const = 'const '
        else:
            const = ''
        return const + self.name



class ArrayType(Type):
    'Represents an array.'

    def __init__(self, name, min, max, const=False):
        'min and max can be None.'
        Type.__init__(self, name, const)
        self.min = min
        self.max = max        


    
class ReferenceType(Type): 
    'A reference type.'    

    def __init__(self, name, const=False, default=None, expandRef=True):
        Type.__init__(self, name, const, default)
        self.expand = expandRef
        
        
    def FullName(self):
        'expand is False for function pointers'
        expand = ' &'
        if not self.expand:
            expand = ''
        return Type.FullName(self) + expand



class PointerType(Type):
    'A pointer type.'
    
    def __init__(self, name, const=False, default=None, expandPointer=False):
        Type.__init__(self, name, const, default)
        self.expand = expandPointer

   
    def FullName(self):
        'expand is False for function pointer'
        expand = ' *'
        if not self.expand:
            expand = ''
        return Type.FullName(self) + expand
   


class FundamentalType(Type): 
    'One of the fundamental types (int, void...).'

    def __init__(self, name, const=False):
        Type.__init__(self, name, const)



class FunctionType(Type):
    'A pointer to a function.'

    def __init__(self, result, params):
        Type.__init__(self, '', False)
        self.result = result
        self.parameters = params
        self.name = self.FullName()


    def FullName(self):
        full = '%s (*)' % self.result.FullName()
        params = [x.FullName() for x in self.parameters]
        full += '(%s)' % ', '.join(params)        
        return full

    

class MethodType(FunctionType):
    'A pointer to a member function of a class.'

    def __init__(self, result, params, class_):
        Type.__init__(self, '', False)
        self.result = result
        self.parameters = params
        self.class_ = class_
        self.name = self.FullName()

    def FullName(self):
        full = '%s (%s::*)' % (self.result.FullName(), self.class_)
        params = [x.FullName() for x in self.parameters]
        full += '(%s)' % ', '.join(params)
        return full



class Variable(Declaration):
    'Represents a global variable.'
    
    def __init__(self, type, name, namespace):
        Declaration.__init__(self, name, namespace)
        # instance of Type
        self.type = type



class ClassVariable(Variable):
    'Represents a class variable.'

    def __init__(self, type, name, class_, visib, static):
        Variable.__init__(self, type, name, None)
        self.visibility = visib
        self.static = static
        self.class_ = class_
    

    def FullName(self):
        return self.class_ + '::' + self.name

        
    
class Enumeration(Declaration):
    
    def __init__(self, name, namespace):
        Declaration.__init__(self, name, namespace)
        self.values = {} # dict of str => int

    def ValueFullName(self, name):
        assert name in self.values
        namespace = self.namespace
        if namespace:
            namespace += '::'
        return namespace + name



class ClassEnumeration(Enumeration):

    def __init__(self, name, class_, visib):
        Enumeration.__init__(self, name, None)
        self.class_ = class_
        self.visibility = visib


    def FullName(self):
        return '%s::%s' % (self.class_, self.name)


    def ValueFullName(self, name):
        assert name in self.values
        return '%s::%s' % (self.class_, name)

    

class Typedef(Declaration):

    def __init__(self, type, name, namespace):
        Declaration.__init__(self, name, namespace)
        self.type = type
        self.visibility = Scope.public


class Union(Declaration):
    'Shallow declaration, because Unions are not supported yet'    
    def __init__(self, name, namespace):
        Declaration.__init__(self, name, namespace)


class ClassUnion(Union):

    def __init__(self, name, class_, visib):
        Union.__init__(self, name, None)
        self.class_ = class_
        self.visibility = visib            

    def FullName(self):
        return '%s::%s' % (self.class_, self.name)


