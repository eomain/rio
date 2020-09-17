
# Rio
A string language generator for the C programming language.
Converts `.rio` definition files into C `.h` files.

## Example usage

###### Create a `greet.rio` file
```
[greet]
en = "hello"
jp = "世界"
```

###### Build the header file
```bash
rio greet.rio -o greet.h
```

###### In the program source
Once we have included the generated header file
we can use the strings in our programs.

```c
printf("%s\n", rio(greet));
```

## License
Distributed under the 2-clause BSD license. See COPYING for more info.
