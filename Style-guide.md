# Style guide

## whitespace

curly brackets start on the same line one space after the function they belong to is declared
ex.
```
loop() {
  thing
}
```

else statements start after the closing curly bracket of an if statement
ex.
```
if(something) {
  code
}else {
  other_code
}
```

lines within curly brackets are given a number of tabs equal to how many curly brackets they are inside of
ex.
```
loop() {
  one_tab() {
    2 tabs
  }
}
```

between two functions there is a newline
ex.
```
void func1() {
  thing
}

void func2() {
  thing
}
```

comments should be allinged and seperated from code
ex.
```
code      //comment
more_code //comment
incorrect//spacing

```

## best practises

use only ASCII characters

avoid having code that is nested more than 3 times, if possible make a sub function
ex.
```
void func1() {
  if(something) {
    while(something else) {
      func2();
    }
  }
}

void func2() {
  if(another thing) {
    code
  }
}
```

if code is reused make it a function
ex.
```
void func1() {
  func2();
  if(something) {
    func2();
  }
}

void func2() {
  code
}
```

always add comments for everything not explicit in the code
ex.
```
int func1() {
  z = x + y;
  func2(z);  //does thing
}

int func2(int z) {
  code
}
```

where possible use c functions instead of c++ functions

avoid using global variables where possible

do not use exeptions

always initailze variables

variables are lowercase with underscores between words
ex.
```
int this_is_an_example_variable = 0;
```

the start of word in a function is capitalized and words are seperated by underscores
ex.
```
void This_Is_An_Example_Function()
```

defines are fuly capitalized with underscores between words
ex.
```
#define THIS_IS_AN_EXAMPLE_DEFINE
```

add ifndef to header files to prevent duplicate inclusion
ex.
```
#ifndef Header_filename
#define Header_filename

  //Header declarations…

#endif
```

more reading https://www.stroustrup.com/JSF-AV-rules.pdf
