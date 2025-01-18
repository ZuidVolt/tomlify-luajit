project(
  'tomlify',
  'c',
  default_options: ['warning_level=3', 'werror=true', 'optimization=3'],
)

# LuaJIT dependency - specifically look in Homebrew path first on macOS
if host_machine.system() == 'darwin'
  luajit_dir = '/opt/homebrew'

  # Add include directories
  add_project_arguments('-I' + luajit_dir + '/include/luajit-2.1', language: 'c')

  # Add library directory
  add_project_link_arguments('-L' + luajit_dir + '/lib', language: 'c')

  luajit_dep = dependency(
    'luajit',
    required: true,
    version: '>=2.1',
    modules: ['luajit-5.1'],
    fallback: ['luajit', 'luajit_dep'],
  )
else
  luajit_dep = dependency(
    'luajit',
    required: true,
    version: '>=2.1',
    fallback: ['luajit', 'luajit_dep'],
  )
endif

# LuaJIT executable
luajit_bin = find_program('luajit', required: true)

# Input files
wrapper_source = 'wrapper.c'
lua_source = 'tomlify.lua'

# Custom target: Generate bytecode header
bytecode_header = custom_target(
  'tomlify_bc',
  input: lua_source,
  output: 'tomlify_bc.h',
  command: [luajit_bin, '-bg', '@INPUT@', '@OUTPUT@'],
)

# Build executable
tomlify_exe = executable(
  'tomlify',
  [wrapper_source, bytecode_header], # Include both source and generated header
  dependencies: [luajit_dep],
  c_args: ['-Wunused-result'],
  include_directories: ['.', 'build'], # Add build directory to include path
  install: true,
)

# Test the build
test('tomlify_build_test', tomlify_exe)
