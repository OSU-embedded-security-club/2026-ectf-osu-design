# Osu Mitre eCTF design

TODO: Explanation


## Usage

Cd to it.

First time you use it, and when reboot, you need to start the devcontainer:
```bash
devcontainer up --workspace-folder .
```

Then to get an interactive enviroment:
```bash
devcontainer exec --workspace-folder . bash
```


From inside the container, use cmake to build:
```
# Configure for mspn
cmake .

# Configure for linux
cmake -DBUILD_MODE=SIMU .

# For either, to compile it
make
```

**NOTE**: When switching modes, **always** clear the cmake cache:
```bash
rm -f CMakeCache.txt
```


### Vim users

For LSP, you must run clangd inside the container. I have provided a `clangd-proxy` script, you must tell nvim to use that ([ex](https://github.com/PsychedelicPalimpsest/dotfiles/blob/fa2a196cacf4af564865a79cebb4f010ad04bc5d/nvim/lua/plugins/lsp.lua#L67)). 

```lua
local local_proxy = vim.fn.getcwd() .. '/clangd-proxy'
local cmd = { "clangd" } -- Default global clangd

-- Check if the proxy script exists and is executable
if vim.fn.executable(local_proxy) == 1 then
cmd = { local_proxy }
end

vim.lsp.config("clangd", {
cmd = cmd,
})
```

