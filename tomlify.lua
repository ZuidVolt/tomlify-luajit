#!/usr/bin/env luajit
local ffi = require("ffi")
---@diagnostic disable: undefined-global
local jit = jit or {} -- Fallback if not running on LuaJIT
if jit.opt then       -- Only optimize if running on LuaJIT
    jit.opt.start("maxmcode=1024", "maxtrace=1000")
end
local C = ffi.C

-- Optimize JIT
jit.opt.start("maxmcode=1024", "maxtrace=1000")

-- FFI declarations
if ffi.os == "Windows" then
    ffi.cdef [[typedef unsigned long size_t;]]
else
    ffi.cdef [[typedef unsigned long size_t;]]
end

ffi.cdef [[
    int access(const char *path, int mode);
    char *realpath(const char *path, char *resolved_path);
    char *strerror(int errnum);
]]

-- Constants
local R_OK, W_OK = 4, 2
local PATH_MAX = 1024
local CHUNK_SIZE = 128 * 1024 -- 128KB chunks

-- Pre-allocated buffers
local path_buffer = ffi.new("char[?]", PATH_MAX)
local copy_buffer = ffi.new("char[?]", CHUNK_SIZE)

-- Logger
local Logger = {
    ERROR = 2,
    level = 2
}

function Logger.error(msg, ...)
    if Logger.level >= Logger.ERROR then
        io.stderr:write(string.format("ERROR: " .. msg .. "\n", ...))
    end
end

-- Error handling
local function create_error(type, message, sys_error)
    local err = {
        type = type,
        message = message,
        sys_error = sys_error and ffi.string(C.strerror(ffi.errno())) or nil
    }

    return setmetatable(err, {
        __tostring = function(self)
            return self.sys_error and
                string.format("%s: %s (%s)", self.type, self.message, self.sys_error) or
                string.format("%s: %s", self.type, self.message)
        end
    })
end

-- File system operations
local FileSystem = {}

function FileSystem.get_real_path(path)
    if not path then return nil end
    local res = C.realpath(path, path_buffer)
    return res ~= nil and ffi.string(res) or nil
end

function FileSystem.verify_permissions(path)
    if C.access(path, R_OK + W_OK) ~= 0 then
        return false, create_error(
            "PermissionDenied",
            string.format("Insufficient permissions for path: %s", path),
            true
        )
    end
    return true
end

function FileSystem.verify_file_health(path)
    local f = io.open(path, "r")
    if not f then
        return false, create_error(
            "InvalidFile",
            string.format("Cannot open file: %s", path),
            true
        )
    end
    local content = f:read("*a")
    f:close()
    return content and #content > 0, create_error("InvalidFile", "File is empty or corrupted")
end

function FileSystem.copy_file(src, dst)
    local fsrc = io.open(src, "rb")
    if not fsrc then
        return false, create_error(
            "FileNotFound",
            string.format("Cannot open source file: %s", src),
            true
        )
    end

    local fdst = io.open(dst, "wb")
    if not fdst then
        fsrc:close()
        return false, create_error(
            "PermissionDenied",
            string.format("Cannot create destination file: %s", dst),
            true
        )
    end

    while true do
        local bytes = fsrc:read(CHUNK_SIZE)
        if not bytes then break end
        if not fdst:write(bytes) then
            fsrc:close()
            fdst:close()
            return false, create_error(
                "OperationFailed",
                "Failed to write to destination file",
                true
            )
        end
    end

    fsrc:close()
    fdst:close()
    return true
end

-- Application logic
local function get_script_dir()
    local src = debug.getinfo(1, "S").source:sub(2)
    return src:match("(.+)/[^/]*$") or "."
end

local function copy_pyproject_toml(template_path, output_dir)
    template_path = template_path or FileSystem.get_real_path(
        get_script_dir() .. "/template/pyproject.toml"
    )
    local dest_path = output_dir and (output_dir .. "/pyproject.toml")
        or (FileSystem.get_real_path(".") .. "/pyproject.toml")

    if not template_path then
        error(create_error("FileNotFound", "Template not found"))
    end

    local ok, err = FileSystem.verify_file_health(template_path)
    if not ok then error(err) end

    ok, err = FileSystem.verify_permissions(template_path)
    if not ok then error(err) end

    if C.access(dest_path, R_OK) == 0 then
        io.write("pyproject.toml already exists. Overwrite? (y/N): ")
        if io.read():lower() ~= "y" then return end

        local backup_path = dest_path .. ".backup"
        local success, err = FileSystem.copy_file(dest_path, backup_path)
        if not success then error(err) end
    end

    local success, err = FileSystem.copy_file(template_path, dest_path)
    if not success then error(err) end

    ok, err = FileSystem.verify_file_health(dest_path)
    if not ok then error(err) end
end

-- Parse command line arguments and execute
local function main()
    local template, output
    local i = 1
    while i <= #arg do
        if arg[i] == "-t" or arg[i] == "--template" then
            template = arg[i + 1]
            i = i + 2
        elseif arg[i] == "-o" or arg[i] == "--output" then
            output = arg[i + 1]
            i = i + 2
        elseif arg[i] == "-h" or arg[i] == "--help" then
            print("Usage: tomlify [-t template] [-o output_dir]")
            print("Options:")
            print("  -t, --template    Path to custom template file")
            print("  -o, --output      Directory to output pyproject.toml")
            os.exit(0)
        else
            i = i + 1
        end
    end

    copy_pyproject_toml(template, output)
end

if not pcall(debug.getlocal, 4, 1) then
    local ok, err = xpcall(main, debug.traceback)
    if not ok then
        Logger.error("%s", tostring(err))
        os.exit(1)
    end
end
