## Context
This repository contains industrial software.
The product is a generic serial terminal emulator, so changes must prioritize robustness, determinism, and maintainability.

## Rules (must follow)

### 1) Naming conventions
Use snake_case and lowercase for all identifiers (variables, functions, structs, typedefs, enums, file names).
Do not introduce CamelCase or PascalCase identifiers.
Macros must use UPPER_SNAKE_CASE.
All file names must also use snake_case and be all lowercase. 
Typedef-defined type names must end with _t.

### 2) Software quality (industrial constraints)
The code must be:
- robust (defensive checks, explicit error handling, no undefined behavior),
- clean and readable,
- portable,
- easy to maintain (small modules, clear APIs, minimal coupling).

### 3) Comments and language
- All code and code comments must be in English.
- The user will communicate in Spanish; you may reply in Spanish, but keep code/comments in English.
- When adding comments, use:
	- Single-line: // comment.
	- Multi-line: use /* ... */ without a leading '*' on each line.

### 4) Scope, simplicity, and feature gating
When asked to create new code, prefer simplicity over complexity.
Focus only on what the user requested.
Do not add extra features, helper functions, or refactors “for completeness” unless:
(a) the user explicitly requests them, OR
(b) you propose them first with a brief justification and wait for explicit approval.

### 5) C/C++ Code Format

#### `switch-case`

Apply this format to all `switch-case` statements in C code:

- `case` labels must have one additional tab of indentation relative to the `switch`.
- Each `case` must wrap its contents with `{}`.
- The final `break;` of the `case` must remain outside the `{}` block.
- There must be a blank line between each `case`.
- `default:` does not need `{}` if it only contains `break;`.
- Do not change the code logic while formatting.
- If multiple `case` labels share the same body, keep the labels grouped and open the block after the last label.
- If a `case` already contains internal `break;` statements inside conditions, preserve them.

Example:

```c
switch (var)
{
	case VAL_0:
	{
	    code();
	    code();
	}
	break;

	case VAL_1:
	{
	    code();
	}
	break;

	default:
	break;
}
```

Example with grouped `case` labels:

```c
switch (var)
{
	case VAL_0:
	case VAL_1:
	case VAL_2:
	{
	    code();
	}
	break;

	default:
	break;
}
```

#### Blank lines before `break` and `return`

When a `break;` or `return;` closes a logic block, leave one blank line before it to visually separate the exit from the rest of the code.

Example with `break;`:

```c
if (condition)
{
    code();
    code();

    break;
}
```

Example with `return;`:

```c
code();
code();

return;
```

This also applies inside `case` blocks, including internal `break;` statements inside conditions.

#### Blank line after `}`

When a block is closed with `}`, leave one blank line before the next executable line of code.

Example:

```c
if (condition)
{
    code();
}

code();
```

This also applies after closing a `switch`, `if`, `else`, `for`, `while`, or a `{}` block inside a `case`.

Exception: do not add a blank line between `}` and `break;` when the `break;` is the formal closing statement of a `case`.

```c
}
break;
```
