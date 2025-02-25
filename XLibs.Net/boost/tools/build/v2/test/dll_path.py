#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

#  Test that the <dll-path> property is correctly set when using
#  <hardcode-dll-paths>true.
from BoostBuild import Tester, List
from string import find


t = Tester()

# The point of this test is to have exe "main" which uses library "b",
# which uses library "a". When "main" is built with <hardcode-dll-paths>true,
# paths to both libraries should be present as values of <dll-path> feature.
# We create a special target type which reports <dll-path> values on its sources
# and compare the list of found values with out expectations.

t.write("Jamfile", """ 
exe main : main.cpp b//b ;
explicit main ;

path-list mp : main ; 
""")

t.write("main.cpp", """ 
int main() { return 0; }

""")

t.write("project-root.jam", """ 
using dll-paths ; 
""")

t.write("dll-paths.jam", """ 
import type ;
import generators ;
import feature ;
import sequence ;
import print ;
import "class" : new ;

rule init ( ) 
{ 
    type.register PATH_LIST : pathlist : : main ;
    
    class dll-paths-list-generator : generator 
    {
        rule __init__ ( )
        {
            generator.__init__ dll-paths.list : EXE : PATH_LIST ;
        }
        
        rule generated-targets ( sources + : property-set : project name ? )        
        {
            local dll-paths ;
            for local s in $(sources)
            {
                local a = [ $(s).action ] ;
                if $(a)
                {
                    local p = [ $(a).properties ] ;
                    dll-paths += [ $(p).get <dll-path> ] ;
                }                                                
            }
            return [ generator.generated-targets $(sources) 
              : [ $(property-set).add-raw $(dll-paths:G=<dll-path>) ] : $(project) $(name) ] ;
            
        }
    }
    generators.register [ new dll-paths-list-generator ] ;
    
}

rule list ( target : sources * : properties * )
{
    local paths = [ feature.get-values <dll-path> : $(properties) ] ;
    paths = [ sequence.insertion-sort $(paths) ] ;
    print.output $(target) ;
    print.text $(paths) ;
}

""")

t.write("a/a.cpp", """ 
void
#if defined(_WIN32)
__declspec(dllexport)
#endif
foo() {}


""")

t.write("a/Jamfile", """ 
lib a : a.cpp ; 
""")

t.write("b/b.cpp", """ 
void
#if defined(_WIN32)
__declspec(dllexport)
#endif
bar() {}


""")

t.write("b/Jamfile", """ 
lib b : b.cpp ../a//a ; 
""")

t.run_build_system("hardcode-dll-paths=true")

t.expect_addition("bin/$toolset/debug/mp.pathlist")

es1 = t.adjust_names(["a/bin/$toolset/debug"])[0]
es2 = t.adjust_names(["b/bin/$toolset/debug"])[0]
content = t.read("bin/$toolset/debug/mp.pathlist")

t.fail_test(find(content, es1) == -1)
t.fail_test(find(content, es2) == -1)

t.cleanup()

