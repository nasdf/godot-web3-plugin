<img src="/logo.png" width="25%" />

# Godot Web3 Plugin

Godot plugin for building web3 games.

## Building

Clone `godot` and copy sources to `modules/web3`.

```
|-- godot
|-- modules
|   |-- web3
```

Build from `godot` root.

```bash
scons custom_modules=../modules platform=osx arch=arm64 --jobs=$(sysctl -n hw.logicalcpu)
```
