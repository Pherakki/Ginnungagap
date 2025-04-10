* NOTE: This is a currently-unreleased library. *

# exbip
`exbip` aims to be an extensible, *zero-cost* serialization abstraction framework over streams for complicated data structures.
The goals of `exbip` are:
- To be *easy to use*:
    - The serialized form of an object is described using a single C++ function that acts as a "schema".
    - Arbitrary, *user-definable* operations are performed using these schemas (such as serialization, deserialization, automatic data offset and size calculation...).
- To be *quick to use*:
    - Write *one* function to handle arbitrary serialization tasks.
    - `exbip` comes with a ready-to-use standard library covering many general-purpose serialization operations.
- To be *easy to extend*:
    - Each operation (serialization, deserialization, etc.) is described by user-definable schema-parser objects.
    - Each task (_e.g._ operate on an int32) is described by "descriptor" mix-in objects.
    - Extend all schema-parsers with the same set of mix-ins.
- To introduce *zero* runtime abstraction overhead over using streams directly, under an optimizing compiler.

## A simple exbip program

## A simple exbip extension

## Contributing
Contributions are very welcome. Before working on a pull request, please consider the following:
- Opening an issue with the `Feature Scoping` tag to propose a feature is recommended before starting work on it, to avoid wasting your time if the feature is better-suited outside of the main repository.
- The framework and standard library are intended to be minimal and lightweight, rather than a monolithic repository of many data formats. The default position is that new descriptors are likely better-placed in their own sub-libraries, although descriptors with large potential user-bases have are well-placed for being merged into the standard library.

