# Tomlify-LuaJIT

A high-performance command-line tool for generating and managing `pyproject.toml` files, built with LuaJIT and C.

## Features

- Fast and efficient `pyproject.toml` file generation
- Custom template support
- Backup creation before overwriting existing files
- Cross-platform compatibility
- Memory-efficient file handling with chunked operations
- Built with LuaJIT for optimal performance

## Installation

### Prerequisites

- LuaJIT 2.1 or higher
- C compiler (gcc, clang, etc.)
- Make or Meson build system

### Building from Source


Using Meson:
```bash
git clone https://github.com/zuidvolt/tomlify-luajit
cd tomlify-luajit
meson setup build
meson compile -C build
```

Using Make (macOS):
```bash
git clone https://github.com/zuidvolt/tomlify-luajit
cd tomlify-luajit
make
```

## Usage

```bash
tomlify [-t template] [-o output_dir]
```

### Options

- `-t, --template`: Specify a custom template file path
- `-o, --output`: Specify output directory for pyproject.toml
- `-h, --help`: Display help information

### Examples

Generate default pyproject.toml in current directory:
```bash
tomlify
```

Use custom template:
```bash
tomlify -t /path/to/custom/template.toml
```

Specify output directory:
```bash
tomlify -o /path/to/project
```

## Features in Detail

- **Safe File Operations**: Automatic backup creation before overwriting existing files
- **Permission Verification**: Checks for proper read/write permissions
- **File Health Checks**: Validates file integrity before and after operations
- **Efficient Memory Usage**: Processes files in chunks to maintain low memory footprint
- **Error Handling**: Comprehensive error reporting with system error details

## Building

### macOS (with Homebrew)

```bash
# Install dependencies
brew install luajit

# Build with make
make

# Or build with meson
meson setup builddir
cd builddir
ninja
```

### Linux

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install luajit libluajit-5.1-dev

# Build with make
make

# Or build with meson
meson setup builddir
cd builddir
ninja
```

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the Apache License, Version 2.0 with important additional terms, including specific commercial use conditions. Users are strongly advised to read the full [LICENSE](LICENSE) file carefully before using, modifying, or distributing this work. The additional terms contain crucial information about liability, data collection, indemnification, and commercial usage requirements that may significantly affect your rights and obligations.

## Acknowledgments

- LuaJIT for the excellent Just-In-Time compiler
- Contributors who helped improve the project

## Project Structure

```
tomlify-luajit/
├── tomlify.lua      # Main LuaJIT implementation
├── wrapper.c        # C wrapper for LuaJIT integration
├── makefile         # Make build configuration
├── meson.build      # Meson build configuration
└── template/        # Default templates directory
```


## Support

For support, please open an issue in the GitHub repository.

---
