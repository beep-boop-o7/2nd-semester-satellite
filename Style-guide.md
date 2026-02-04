# Style guide

## whitespace

curly brackets start on the same line one space after the function they belong to is declared
ex.
```
loop {
  thing
}
```

lines within curly brackets are given a number of tabs equal to how many curly brackets they are inside of
ex.
```
loop {
  one tab {
    2 tabs
  }
}
```

between two functions there is a newline
ex.
```
func1 {
  thing
}

func2 {
  thing
}
```

comments have a space before the double slash
ex.
```
code //comment
```

## best practises

avoid having code that is nested more than 3 times, if possible make a sub function
ex.
```
func1 {
  if(something) {
    while(something else) {
      func2()
    }
  }
}

func2 {
  if(another thing) {
    code
  }
}
```

if code is reused make it a function
ex.
```
func1 {
  func2()
  if(something) {
    func2()
  }
}

func2 {
  code
}
```

always add comments for everything not explicit in the code
ex.
```
func1 {
  z = x + y;
  func2(z); //does thing
}

func2(int z) {
  code
}
```
