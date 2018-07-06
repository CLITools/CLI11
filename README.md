[![Build Status Linux and macOS][travis-badge]][Travis]
[![Build Status Windows][appveyor-badge]][AppVeyor]
[![Code Coverage][codecov-badge]][CodeCov]
[![Codacy Badge][codacy-badge]][codacy-link]
[![Join the chat at https://gitter.im/CLI11gitter/Lobby][gitter-badge]][gitter]
[![License: BSD][license-badge]](./LICENSE)
[![Latest release][releases-badge]][Github Releases]
[![DOI][DOI-badge]][DOI-link]
[![Conan.io][conan-badge]][conan-link]
[![Try CLI11 1.5.2 online][wandbox-badge]][wandbox-link]

[Documentation][GitBook] •
[API Reference][api-docs] •
[What's new](./CHANGELOG.md)

# CLI11: Command line parser for C++11

CLI11 provides all the features you expect in a powerful command line parser, with a beautiful, minimal syntax and no dependencies beyond C++11. It is header only, and comes in a single file form for easy inclusion in projects. It is easy to use for small projects, but powerful enough for complex command line projects, and can be customized for frameworks.
It is tested on [Travis] and [AppVeyor], and is being included in the [GooFit GPU fitting framework][GooFit]. It was inspired by [`plumbum.cli`][Plumbum] for Python. CLI11 has a user friendly introduction in this README, a more in-depth tutorial [GitBook], as well as [API documentation][api-docs] generated by Travis.
See the [changelog](./CHANGELOG.md) or [GitHub Releases] for details for current and past releases. Also see the [Version 1.0 post], [Version 1.3 post], or [Version 1.6 post] for more information.

You can be notified when new releases are made by subscribing to https://github.com/CLIUtils/CLI11/releases.atom on an RSS reader, like Feedly.

### Why write another CLI parser?

An acceptable CLI parser library should be all of the following:

* Easy to include (i.e., header only, one file if possible, **no external requirements**).
* Short, simple syntax: This is one of the main reasons to use a CLI parser, it should make variables from the command line nearly as easy to define as any other variables. If most of your program is hidden in CLI parsing, this is a problem for readability.
* C++11 or better: Should work with GCC 4.7+ (such as GCC 4.8 on CentOS 7), Clang 3.5+, AppleClang 7+, NVCC 7.0+, or MSVC 2015+.
* Work on Linux, macOS, and Windows.
* Well tested using [Travis] (Linux and macOS) and [AppVeyor] (Windows). "Well" is defined as having good coverage measured by [CodeCov].
* Clear help printing.
* Nice error messages.
* Standard shell idioms supported naturally, like grouping flags, a positional separator, etc.
* Easy to execute, with help, parse errors, etc. providing correct exit and details.
* Easy to extend as part of a framework that provides "applications" to users.
* Usable subcommand syntax, with support for multiple subcommands, nested subcommands, and optional fallthrough (explained later).
* Ability to add a configuration file (`ini` format), and produce it as well.
* Produce real values that can be used directly in code, not something you have pay compute time to look up, for HPC applications.
* Work with standard types, simple custom types, and extendible to exotic types.
* Permissively licensed.

<details><summary>The major CLI parsers for C++ include, with my biased opinions: (click to expand)</summary><p>

| Library | My biased opinion |
|---------|-------------------|
| [Boost Program Options] | A great library if you already depend on Boost, but its pre-C++11 syntax is really odd and setting up the correct call in the main function is poorly documented (and is nearly a page of code). A simple wrapper for the Boost library was originally developed, but was discarded as CLI11 became more powerful. The idea of capturing a value and setting it originated with Boost PO. [See this comparison.][cli11-po-compare] |
| [The Lean Mean C++ Option Parser] | One header file is great, but the syntax is atrocious, in my opinion. It was quite impractical to wrap the syntax or to use in a complex project. It seems to handle standard parsing quite well. |
| [TCLAP] | The not-quite-standard command line parsing causes common shortcuts to fail. It also seems to be poorly supported, with only minimal bugfixes accepted. Header only, but in quite a few files. Has not managed to get enough support to move to GitHub yet. No subcommands. Produces wrapped values. |
| [Cxxopts] | C++11, single file, and nice CMake support, but requires regex, therefore GCC 4.8 (CentOS 7 default) does not work. Syntax closely based on Boost PO, so not ideal but familiar. |
| [DocOpt] | Completely different approach to program options in C++11, you write the docs and the interface is generated. Too fragile and specialized. |

After I wrote this, I also found the following libraries:

| Library | My biased opinion |
|---------|-------------------|
| [GFlags] | The Google Commandline Flags library. Uses macros heavily, and is limited in scope, missing things like subcommands. It provides a simple syntax and supports config files/env vars. |
| [GetOpt] | Very limited C solution with long, convoluted syntax. Does not support much of anything, like help generation. Always available on UNIX, though (but in different flavors). |
| [ProgramOptions.hxx] | Intresting library, less powerful and no subcommands. Nice callback system. |
| [Args] | Also interesting, and supports subcommands. I like the optional-like design, but CLI11 is cleaner and provides direct value access, and is less verbose. |
| [Argument Aggregator] | I'm a big fan of the [fmt] library, and the try-catch statement looks familiar.  :thumbsup: Doesn't seem to support subcommands. |
| [Clara] | Simple library built for the excellent [Catch] testing framework. Unique syntax, limited scope. |
| [Argh!] | Very minimalistic C++11 parser, single header. Don't have many features. No help generation?!?! At least it's exception-free.|
| [CLI]   | Custom language and parser. Huge build-system overkill for very little benefit. Last release in 2009, but still occasionally active. |

See [Awesome C++] for a less-biased list of parsers. You can also find other single file libraries at [Single file libs].

</p></details>
<br/>

None of these libraries fulfill all the above requirements, or really even come close. As you probably have already guessed, CLI11 does.
So, this library was designed to provide a great syntax, good compiler compatibility, and minimal installation fuss.

## Features not supported by this library

There are some other possible "features" that are intentionally not supported by this library:

* Non-standard variations on syntax, like `-long` options. This is non-standard and should be avoided, so that is enforced by this library.
* Completion of partial options, such as Python's `argparse` supplies for incomplete arguments. It's better not to guess. Most third party command line parsers for python actually reimplement command line parsing rather than using argparse because of this perceived design flaw.
* In C++14, you could have a set of `callback` methods with differing signatures (tested in a branch). Not deemed worth having a C++14 variation on API and removed.
* Autocomplete: This might eventually be added to both Plumbum and CLI11, but it is not supported yet.
* Wide strings / unicode: Since this uses the standard library only, it might be hard to properly implement, but I would be open to suggestions in how to do this.


## Installing

To use, there are two methods:

1. Copy `CLI11.hpp` from the [most recent release][Github Releases] into your include directory, and you are set. This is combined from the source files  for every release. This includes the entire command parser library, but does not include separate utilities (like `Timer`, `AutoTimer`). The utilities are completely self contained and can be copied separately.
2. Use `CLI/*.hpp` files. You could check out the repository as a submodule, for example. You can use the `CLI11::CLI11` interface target when linking from `add_subdirectory`.
   You can also configure and optionally install the project, and then use `find_package(CLI11 CONFIG)` to get the `CLI11::CLI11` target. You can also use [Conan.io][conan-link] or [Hunter].
   (These are just conveniences to allow you to use your favorite method of managing packages; it's just header only so including the correct path and
   using C++11 is all you really need.)

To build the tests, checkout the repository and use CMake:

```bash
mkdir build
cd build
cmake ..
make
GTEST_COLOR=1 CTEST_OUTPUT_ON_FAILURE=1 make test
```

## Adding options

To set up, add options, and run, your main function will look something like this:

```cpp
CLI::App app{"App description"};

std::string filename = "default";
app.add_option("-f,--file", filename, "A help string");

CLI11_PARSE(app, argc, argv);
```

<details><summary>Note: If you don't like macros, this is what that macro expands to: (click to expand)</summary><p>

```cpp
try {
    app.parse(argc, argv);
} catch (const CLI::ParseError &e) {
    return app.exit(e);
}
```

The try/catch block ensures that `-h,--help` or a parse error will exit with the correct return code (selected from `CLI::ExitCodes`). (The return here should be inside `main`). You should not assume that the option values have been set inside the catch block; for example, help flags intentionally short-circuit all other processing for speed and to ensure required options and the like do not interfere. 

</p></details>
</br>


The initialization is just one line, adding options is just two each. The parse macro is just one line (or 5 for the contents of the macro).  After the app runs, the filename will be set to the correct value if it was passed, otherwise it will be set to the default. You can check to see if this was passed on the command line with `app.count("--file")`.

The supported values are:

```cpp
app.add_option(option_name,
               variable_to_bind_to, // int, float, vector, or string-like
               help_string="",
               default=false)

app.add_complex(... // Special case: support for complex numbers

app.add_flag(option_name,
             int_or_bool = nothing,
             help_string="")

app.add_flag_function(option_name,
             function <void(size_t count)>,
             help_string="")

app.add_set(option_name,
            variable_to_bind_to,
            set_of_possible_options,
            help_string="",
            default=false)

app.add_set_ignore_case(... // String only

App* subcom = app.add_subcommand(name, description);
```

An option name must start with a alphabetic character or underscore. For long options, anything but an equals sign or a comma is valid after that. Names are given as a comma separated string, with the dash or dashes. An option or flag can have as many names as you want, and afterward, using `count`, you can use any of the names, with dashes as needed, to count the options. One of the names is allowed to be given without proceeding dash(es); if present the option is a positional option, and that name will be used on help line for its positional form. If you want the default value to print in the help description, pass in `true` for the final parameter for `add_option` or `add_set`. The set options allow your users to pick from a set of predefined options; you can add an existing set if you need to modify the set later, or you can use an initializer list.

On a C++14 compiler, you can pass a callback function directly to `.add_flag`, while in C++11 mode you'll need to use `.add_flag_function` if you want a callback function. The function will be given the number of times the flag was passed. You can throw a relevant `CLI::ParseError` to signal a failure.

On a compiler that supports C++17's `__has_include`, you can also use `std::optional`, `std::experimental::optional`, and `boost::optional` directly in an `add_option` call. If you don't have `__has_include`, you can define `CLI11_BOOST_OPTIONAL 1` before including CLI11 to manually add support (or 0 to remove) for `boost::optional`. See [CLI11 Internals] for information on how this was done and how you can add your own converters.

### Example

* `"one,-o,--one"`: Valid as long as not a flag, would create an option that can be specified positionally, or with `-o` or `--one`
* `"this"` Can only be passed positionally
* `"-a,-b,-c"` No limit to the number of non-positional option names


The add commands return a pointer to an internally stored `Option`. If you set the final argument to true, the default value is captured and printed on the command line with the help flag. This option can be used directly to check for the count (`->count()`) after parsing to avoid a string based lookup. Before parsing, you can set the following options:

* `->required()`: The program will quit if this option is not present. This is `mandatory` in Plumbum, but required options seems to be a more standard term. For compatibility, `->mandatory()` also works.
* `->expected(N)`: Take `N` values instead of as many as possible, only for vector args. If negative, require at least `-N`; end with `--` or another recognized option.
* `->type_name(typename)`: Set the name of an Option's type (`type_name_fn` allows a function instead)
* `->type_size(N)`: Set the intrinsic size of an option. The parser will require multiples of this number if negative.
* `->needs(opt)`: This option requires another option to also be present, opt is an `Option` pointer.
* `->excludes(opt)`: This option cannot be given with `opt` present, opt is an `Option` pointer.
* `->envname(name)`: Gets the value from the environment if present and not passed on the command line.
* `->group(name)`: The help group to put the option in. No effect for positional options. Defaults to `"Options"`. `""` will not show up in the help print (hidden).
* `->ignore_case()`: Ignore the case on the command line (also works on subcommands, does not affect arguments).
* `->multi_option_policy(CLI::MultiOptionPolicy::Throw)`: Set the multi-option policy. Shortcuts available: `->take_last()`, `->take_first()`, and `->join()`. This will only affect options expecting 1 argument or bool flags (which always default to take last).
* `->check(CLI::ExistingFile)`: Requires that the file exists if given.
* `->check(CLI::ExistingDirectory)`: Requires that the directory exists.
* `->check(CLI::ExistingPath)`: Requires that the path (file or directory) exists.
* `->check(CLI::NonexistentPath)`: Requires that the path does not exist.
* `->check(CLI::Range(min,max))`: Requires that the option be between min and max (make sure to use floating point if needed). Min defaults to 0.
* `->transform(std::string(std::string))`: Converts the input string into the output string, in-place in the parsed options.
* `->each(void(std::string)>`: Run this function on each value received, as it is received.
* `->configurable(false)`: Disable this option from being in a configuration file.

These options return the `Option` pointer, so you can chain them together, and even skip storing the pointer entirely. Check takes any function that has the signature `void(const std::string&)`; it should throw a `ValidationError` when validation fails. The help message will have the name of the parent option prepended. Since `check` and `transform` use the same underlying mechanism, you can chain as many as you want, and they will be executed in order. If you just want to see the unconverted values, use `.results()` to get the `std::vector<std::string>` of results. Validate can also be a subclass of `CLI::Validator`, in which case it can also set the type name and can be combined with `&` and `|` (all built-in validators are this sort).


On the command line, options can be given as:

* `-a` (flag)
* `-abc` (flags can be combined)
* `-f filename` (option)
* `-ffilename` (no space required)
* `-abcf filename` (flags and option can be combined)
* `--long` (long flag)
* `--file filename` (space)
* `--file=filename` (equals)

Extra positional arguments will cause the program to exit, so at least one positional option with a vector is recommended if you want to allow extraneous arguments.
If you set `.allow_extras()` on the main `App`, you will not get an error. You can access the missing options using `remaining` (if you have subcommands, `app.remaining(true)` will get all remaining options, subcommands included).

You can access a vector of pointers to the parsed options in the original order using `parse_order()`.
If `--` is present in the command line that does not end an unlimited option, then
everything after that is positional only.



## Subcommands

Subcommands are supported, and can be nested infinitely. To add a subcommand, call the `add_subcommand` method with a name and an optional description. This gives a pointer to an `App` that behaves just like the main app, and can take options or further subcommands. Add `->ignore_case()` to a subcommand to allow any variation of caps to also be accepted. Children inherit the current setting from the parent. You cannot add multiple matching subcommand names at the same level (including ignore
case).

If you want to require that at least one subcommand is given, use `.require_subcommand()` on the parent app. You can optionally give an exact number of subcommands to require, as well. If you give two arguments, that sets the min and max number allowed.
0 for the max number allowed will allow an unlimited number of subcommands. As a handy shortcut, a single negative value N will set "up to N" values. Limiting the maximimum number allows you to keep arguments that match a previous
subcommand name from matching.

If an `App` (main or subcommand) has been parsed on the command line, `->parsed` will be true (or convert directly to bool).
All `App`s have a `get_subcommands()` method, which returns a list of pointers to the subcommands passed on the command line. A `got_subcommand(App_or_name)` method is also provided that will check to see if an `App` pointer or a string name was collected on the command line.

For many cases, however, using an app's callback may be easier. Every app executes a callback function after it parses; just use a lambda function (with capture to get parsed values) to `.callback`. If you throw `CLI::Success` or `CLI::RuntimeError(return_value)`, you can
even exit the program through the callback. The main `App` has a callback slot, as well, but it is generally not as useful.
You are allowed to throw `CLI::Success` in the callbacks.
Multiple subcommands are allowed, to allow [`Click`][Click] like series of commands (order is preserved).

There are several options that are supported on the main app and subcommands. These are:

* `.ignore_case()`: Ignore the case of this subcommand. Inherited by added subcommands, so is usually used on the main `App`.
* `.fallthrough()`: Allow extra unmatched options and positionals to "fall through" and be matched on a parent command. Subcommands always are allowed to fall through.
* `.require_subcommand()`: Require 1 or more subcommands.
* `.require_subcommand(N)`: Require `N` subcommands if `N>0`, or up to `N` if `N<0`. `N=0` resets to the default 0 or more.
* `.require_subcommand(min, max)`: Explicitly set min and max allowed subcommands. Setting `max` to 0 is unlimited.
* `.add_subcommand(name, description="")` Add a subcommand, returns a pointer to the internally stored subcommand.
* `.got_subcommand(App_or_name)`: Check to see if a subcommand was received on the command line.
* `.get_subcommands(filter)`: The list of subcommands given on the command line.
* `.get_parent()`: Get the parent App or nullptr if called on master App.
* `.get_option(name)`: Get an option pointer by option name
* `.get_options(filter)`: Get the list of all defined option pointers (useful for processing the app for custom output formats).
* `.parse_order()`: Get the list of option pointers in the order they were parsed (including duplicates).
* `.formatter(fmt)`: Set a formatter, with signature `std::string(const App*, std::string, AppFormatMode)`. See Formatting for more details.
* `.get_description()`: Access the description.
* `.parsed()`: True if this subcommand was given on the command line.
* `.name(name)`: Add or change the name.
* `.callback(void() function)`: Set the callback that runs at the end of parsing. The options have already run at this point.
* `.allow_extras()`: Do not throw an error if extra arguments are left over.
* `.prefix_command()`: Like `allow_extras`, but stop immediately on the first unrecognised item. It is ideal for allowing your app or subcommand to be a "prefix" to calling another app.
* `.footer(message)`: Set text to appear at the bottom of the help string.
* `.set_help_flag(name, message)`: Set the help flag name and message, returns a pointer to the created option.
* `.set_help_all_flag(name, message)`: Set the help all flag name and message, returns a pointer to the created option. Expands subcommands.
* `.failure_message(func)`: Set the failure message function. Two provided: `CLI::FailureMessage::help` and `CLI::FailureMessage::simple` (the default).
* `.group(name)`: Set a group name, defaults to `"Subcommands"`. Setting `""` will be hide the subcommand.

> Note: if you have a fixed number of required positional options, that will match before subcommand names. `{}` is an empty filter function.

## Configuration file

```cpp
app.set_config(option_name="",
               default_file_name="",
               help_string="Read an ini file",
               required=false)
```

If this is called with no arguments, it will remove the configuration file option (like `set_help_flag`). Setting a configuration option is special. If it is present, it will be read along with the normal command line arguments. The file will be read if it exists, and does not throw an error unless `required` is `true`. Configuration files are in `ini` format by default (other formats can be added by an adept user). An example of a file:

```ini
; Commments are supported, using a ;
; The default section is [default], case insensitive

value = 1
str = "A string"
vector = 1 2 3

; Section map to subcommands
[subcommand]
in_subcommand = Wow
sub.subcommand = true
```

Spaces before and after the name and argument are ignored. Multiple arguments are separated by spaces. One set of quotes will be removed, preserving spaces (the same way the command line works). Boolean options can be `true`, `on`, `1`, `yes`; or `false`, `off`, `0`, `no` (case insensitive). Sections (and `.` separated names) are treated as subcommands (note: this does not mean that subcommand was passed, it just sets the "defaults". To print a configuration file from the passed
arguments, use `.config_to_str(default_also=false, prefix="", write_description=false)`, where `default_also` will also show any defaulted arguments, `prefix` will add a prefix, and `write_description` will include option descriptions.

## Inheriting defaults

Many of the defaults for subcommands and even options are inherited from their creators. The inherited default values for subcommands are `allow_extras`, `prefix_command`, `ignore_case`, `fallthrough`, `group`, `footer`, and maximum number of required subcommands. The help flag existence, name, and description are inherited, as well.

Options have defaults for `group`, `required`, `multi_option_policy`, and `ignore_case`. To set these defaults, you should set the `option_defaults()` object, for example:

```cpp
app.option_defaults()->required();
// All future options will be required
```

The default settings for options are inherited to subcommands, as well.

## Formatting

The job of formatting help printouts is delegated to a formatter callable object on Apps and Options. You are free to replace either formatter by calling `formatter(fmt)` on an `App`, where fmt is any copyable callable with the correct signature. 
CLI11 comes with a default App formatter functional, `Formatter`. It is customizable; you can set `label(key, value)` to replace the default labels like `REQUIRED`, and `column_width(n)` to set the width of the columns before you add the functional to the app or option. You can also override almost any stage of the formatting process in a subclass of either formatter. If you want to make a new formatter from scratch, you can do
that too; you just need to implement the correct signature. The first argument is a const pointer to the in question. The formatter will get a `std::string` usage name as the second option, and a `AppFormatMode` mode for the final option. It should return a `std::string`.

The `AppFormatMode` can be `Normal`, `All`, or `Sub`, and it indicates the situation the help was called in. `Sub` is optional, but the default formatter uses it to make sure expanded subcommands are called with
their own formatter since you can't access anything but the call operator once a formatter has been set.

## Subclassing

The App class was designed allow toolkits to subclass it, to provide preset default options (see above) and setup/teardown code. Subcommands remain an unsubclassed `App`, since those are not expected to need setup and teardown. The default `App` only adds a help flag, `-h,--help`, than can removed/replaced using `.set_help_flag(name, help_string)`. You can also set a help-all flag with `.set_help_all_flag(name, help_string)`; this will expand the subcommands (one level only). You can remove options if you have pointers to them using `.remove_option(opt)`. You can add a `pre_callback` override to customize the after parse
but before run behavior, while
still giving the user freedom to `callback` on the main app.

The most important parse function is `parse(std::vector<std::string>)`, which takes a reversed list of arguments (so that `pop_back` processes the args in the correct order). `get_help_ptr` and `get_config_ptr` give you access to the help/config option pointers. The standard `parse` manually sets the name from the first argument, so it should not be in this vector.

Also, in a related note, the `App` you get a pointer to is stored in the parent `App` in a `unique_ptr`s (like `Option`s) and are deleted when the main `App` goes out of scope.


## How it works

Every `add_` option you have seen so far depends on one method that takes a lambda function. Each of these methods is just making a different lambda function with capture to populate the option. The function has full access to the vector of strings, so it knows how many times an option was passed or how many arguments it received (flags add empty strings to keep the counts correct). The lambda returns `true` if it could validate the option strings, and
`false` if it failed.

Other values can be added as long as they support `operator>>` (and defaults can be printed if they support `operator<<`). To add an enum, for example, provide a custom `operator>>` with an `istream` (inside the CLI namespace is fine if you don't want to interfere with an existing `operator>>`).

If you wanted to extend this to support a completely new type, just use a lambda. An example of a new parser for `complex<double>` that supports all of the features of a standard `add_options` call is in [one of the tests](./tests/NewParseTest.cpp). A simpler example is shown below:

### Example

```cpp
app.add_option("--fancy-count", [](std::vector<std::string> val){
    std::cout << "This option was given " << val.size() << " times." << std::endl
    });
```

## Utilities

There are a few other utilities that are often useful in CLI programming. These are in separate headers, and do not appear in `CLI11.hpp`, but are completely independent and can be used as needed. The `Timer`/`AutoTimer` class allows you to easily time a block of code, with custom print output. 

```cpp
{
CLI::AutoTimer timer {"My Long Process", CLI::Timer::Big};
some_long_running_process();
}
```

This will create a timer with a title (default: `Timer`), and will customize the output using the predefined `Big` output (default: `Simple`). Because it is an `AutoTimer`, it will print out the time elapsed when the timer is destroyed at the end of the block. If you use `Timer` instead, you can use `to_string` or `std::cout << timer << std::endl;` to print the time. The print function can be any function that takes two strings, the title and the time, and returns a formatted
string for printing.

## Other libraries

If you use the excellent [Rang] library to add color to your terminal in a safe, multi-platform way, you can combine it with CLI11 nicely:

```cpp
std::atexit([](){std::cout << rang::style::reset;});
try {
    app.parse(argc, argv);
} catch (const CLI::ParseError &e) {
    std::cout << (e.get_exit_code()==0 ? rang::fg::blue : rang::fg::red);
    return app.exit(e);
}
```

This will print help in blue, errors in red, and will reset before returning the terminal to the user.

If you are on a Unix-like system, and you'd like to handle control-c and color, you can add:

```cpp
 #include <csignal>
 void signal_handler(int s) {
     std::cout << std::endl << rang::style::reset << rang::fg::red << rang::fg::bold;
     std::cout << "Control-C detected, exiting..." << rang::style::reset << std::endl;
     std::exit(1); // will call the correct exit func, no unwinding of the stack though
 }
```

And, in your main function:

```cpp
     // Nice Control-C
     struct sigaction sigIntHandler;
     sigIntHandler.sa_handler = signal_handler;
     sigemptyset(&sigIntHandler.sa_mask);
     sigIntHandler.sa_flags = 0;
     sigaction(SIGINT, &sigIntHandler, nullptr);
```

## Contributing

To contribute, open an [issue][Github Issues] or [pull request][Github Pull Requests] on GitHub, or ask a question on [gitter]. The is also a short note to contributors [here](./CONTRIBUTING.md).

As of version 1.0, this library is available under a 3-Clause BSD license. See the [LICENSE](./LICENSE) file for details.

This project was created by [Henry Schreiner](https://github.com/henryiii).
Significant features and/or improvements to the code were contributed by:

* [Anton](https://github.com/SX91)
* [Doug Johnston](https://github.com/dvj)
* [Jonas Nilsson](https://github.com/SkyToGround)
* [Lucas Czech](https://github.com/lczech)
* [Marcus Brinkmann](https://github.com/lambdafu)
* [Mathias Soeken](https://github.com/msoeken)
* [Nathan Hourt](https://github.com/nathanhourt)
* [Sean Fisk](https://github.com/seanfisk)
* [Stéphane Del Pino](https://github.com/delpinux)

CLI11 was developed at the [University of Cincinnati] to support of the [GooFit] library under [NSF Award 1414736]. Version 0.9 was featured in a [DIANA/HEP] meeting at CERN ([see the slides][DIANA slides]). Please give it a try! Feedback is always welcome.


[DOI-badge]:             https://zenodo.org/badge/80064252.svg
[DOI-link]:              https://zenodo.org/badge/latestdoi/80064252
[travis-badge]:          https://img.shields.io/travis/CLIUtils/CLI11/master.svg?label=Linux/macOS
[Travis]:                https://travis-ci.org/CLIUtils/CLI11
[appveyor-badge]:        https://img.shields.io/appveyor/ci/HenrySchreiner/cli11/master.svg?label=Windows
[AppVeyor]:              https://ci.appveyor.com/project/HenrySchreiner/cli11
[codecov-badge]:         https://codecov.io/gh/CLIUtils/CLI11/branch/master/graph/badge.svg
[CodeCov]:               https://codecov.io/gh/CLIUtils/CLI11
[gitter-badge]:          https://badges.gitter.im/CLI11gitter/Lobby.svg
[gitter]:                https://gitter.im/CLI11gitter/Lobby
[license-badge]:         https://img.shields.io/badge/License-BSD-blue.svg
[conan-badge]:           https://api.bintray.com/packages/cliutils/CLI11/CLI11%3Acliutils/images/download.svg
[conan-link]:            https://bintray.com/cliutils/CLI11/CLI11%3Acliutils/_latestVersion
[Github Releases]:       https://github.com/CLIUtils/CLI11/releases
[Github Issues]:         https://github.com/CLIUtils/CLI11/issues
[Github Pull Requests]:  https://github.com/CLIUtils/CLI11/pulls
[GooFit]:                https://GooFit.github.io 
[Plumbum]:               https://plumbum.readthedocs.io/en/latest/
[Click]:                 http://click.pocoo.org
[api-docs]:              https://CLIUtils.github.io/CLI11/index.html
[Rang]:                  https://github.com/agauniyal/rang
[Boost Program Options]: http://www.boost.org/doc/libs/1_63_0/doc/html/program_options.html
[The Lean Mean C++ Option Parser]: http://optionparser.sourceforge.net
[TCLAP]:                 http://tclap.sourceforge.net
[Cxxopts]:               https://github.com/jarro2783/cxxopts
[DocOpt]:                https://github.com/docopt/docopt.cpp
[ROOT]:                  https://root.cern.ch
[cltools-cmake]:         https://github.com/CLIUtils/cmake
[GFlags]:                https://gflags.github.io/gflags
[GetOpt]:                https://www.gnu.org/software/libc/manual/html_node/Getopt.html
[DIANA/HEP]:             http://diana-hep.org
[NSF Award 1414736]:     https://nsf.gov/awardsearch/showAward?AWD_ID=1414736
[University of Cincinnati]: http://www.uc.edu
[GitBook]:               https://cliutils.gitlab.io/CLI11Tutorial
[CLI11 Internals]:       https://cliutils.gitlab.io/CLI11Tutorial/chapters/internals.html
[ProgramOptions.hxx]:    https://github.com/Fytch/ProgramOptions.hxx
[Argument Aggregator]:   https://github.com/vietjtnguyen/argagg
[Args]:                  https://github.com/Taywee/args
[Argh!]:                 https://github.com/adishavit/argh 
[fmt]:                   https://github.com/fmtlib/fmt
[Catch]:                 https://github.com/philsquared/Catch
[Clara]:                 https://github.com/philsquared/Clara
[Version 1.0 post]:      https://iscinumpy.gitlab.io/post/announcing-cli11-10/
[Version 1.3 post]:      https://iscinumpy.gitlab.io/post/announcing-cli11-13/
[Version 1.6 post]:      https://iscinumpy.gitlab.io/post/announcing-cli11-16/
[wandbox-badge]:         https://img.shields.io/badge/try-online-blue.svg
[wandbox-link]:          https://wandbox.org/permlink/95DStlyHsquYH3k6
[releases-badge]:        https://img.shields.io/github/release/CLIUtils/CLI11.svg
[cli11-po-compare]:      https://iscinumpy.gitlab.io/post/comparing-cli11-and-boostpo/
[DIANA slides]:          https://indico.cern.ch/event/619465/contributions/2507949/attachments/1448567/2232649/20170424-diana-2.pdf
[Awesome C++]:           https://github.com/fffaraz/awesome-cpp/blob/master/README.md#cli
[CLI]:                   https://codesynthesis.com/projects/cli/
[Single file libs]:      https://github.com/nothings/single_file_libs/blob/master/README.md
[codacy-badge]:          https://api.codacy.com/project/badge/Grade/ac0df3aead2a4421b02070c3f324a0b9
[codacy-link]:           https://www.codacy.com/app/henryiii/CLI11?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=CLIUtils/CLI11&amp;utm_campaign=Badge_Grade
[Hunter]:                https://docs.hunter.sh/en/latest/packages/pkg/CLI11.html
