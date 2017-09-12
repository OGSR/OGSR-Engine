C++ to HTML converter

This sample stylizes C++ code using HTML. The generated HTML file uses CSS
classes to allow flexibility and easy tweaking of color/font schemes (see
the sample CSS file "style.css"). Try compiling "cpp_to_html.cpp" and make
the compiled application parse its source ("cpp_to_html.cpp"). The
generated HTML file will be "cpp_to_html.cpp.html". You can then view the
generated html. Make sure that the "style.css" is in the same directory as
the generated html file. or tweak the html source to suit.

The application can accept multiple files from the command line. Input
files are assumed to be valid C++ files otherwise parsing will be
terminated as soon as an invalid syntax is encountered.

Cheers,
--Joel de Guzman

As an aside - this example parses C#, Java and JScript just as well as
C/C++ due to their nearly identical lexical structure. --Carl Daniel




