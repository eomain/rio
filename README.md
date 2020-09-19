
# Rio
A string language generator for the C programming language.
Converts `.rio` definition files into C `.h` files.

## Example usage

###### Create a `greet.rio` file
```
[greet]
en = "hello"
ja = "こんにちは"
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

## Build
Rio is self-hosting. To build with any supported language as the default,
the ISO 639 language code must be supplied as a compile-time argument.

```bash
# Change directory
cd rio

# Init libraries
git submodule update --init --recursive

# Build rio using `LANG` option (defaults to 'en' if omitted)
make LANG=ja
```

## License
Distributed under the 2-clause BSD license. See COPYING for more info.
