# 351-redirect

## Compilation

To compile the program, use GCC:

```bash
gcc -o redir redirect_streams.c
```

## Usage

The basic syntax is:

```bash
./redir <input> <command> <output>
```

Where:
- `<input>`: Input file path or "-" for standard input
- `<command>`: Command to execute (with optional arguments)
- `<output>`: Output file path or "-" for standard output

### Examples

1. Sort a file:
```bash
./redir input.txt "sort" output.txt
```

2. Count lines in a file:
```bash
./redir input.txt "wc -l" output.txt
```

3. Use standard input:
```bash
cat input.txt | ./redir - "sort" output.txt
```

4. Use standard output:
```bash
./redir input.txt "sort" -
```

5. Use both standard input and output:
```bash
cat input.txt | ./redir - "sort" -
```

## Examples with Expected Output

```bash
# Create a test file
echo -e "zebra\napple\nbanana" > test.txt

# Sort the file
./redir test.txt "sort" sorted.txt
cat sorted.txt
# Output:
# apple
# banana
# zebra

# Count lines
./redir test.txt "wc -l" count.txt
cat count.txt
# Output: 3

# Filter lines
./redir test.txt "grep a" filtered.txt
cat filtered.txt
# Output:
# apple
# banana
```