## wgmask
`wgmask` is a client-server UDP proxy to mask the WireGuard traffic to bypass the DPI systems

## How it works
Wireguard requests go through such a path:<br>
`<WireGuard client>` -> `<wgmask server>` -> `<DPI>` -> `<wgmask server>` -> `<WireGuard server>`<br>
Wireguard responses go through such a path:<br>
`<WireGuard server>` -> `<wgmask server>` -> `<DPI>` -> `<wgmask client>` -> `<WireGuard client>`

Before passing through `DPI`, packets are masked using `wgmask`, and after passing through `DPI`, they are unmasked.

## How to set up
**!IMPORTANT!** You must have control over the server
- First step: run `wgmask -s -l 0.0.0.0:<random port> -r 127.0.0.1:<WireGuard port>` on the server. You can daemonize this `wgmask` process
- Run `wgmask -c -l 0.0.0.0:<random port> -r <server ip>:<server's wgmask port>` on the client
- Then you need to replcae the endpoint in the **client WireGuard client**: replace `Endpoint = <server ip>:<some port>` -> `Endpoint = 127.0.0.1:<client's wgmask port>`

## How to install
#### Downloading
Download the [lastest release](https://github.com/Evilur/wgmask/releases/latest) for your operating system and your architecture
#### Building from source
```SHELL
git clone https://github.com/Evilur/wgmask.git
cd wgmask
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/<toolchain file>
cmake --build build
```
