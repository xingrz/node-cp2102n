node-cp2102n [![build](https://github.com/xingrz/node-cp2102n/actions/workflows/build.yml/badge.svg)](https://github.com/xingrz/node-cp2102n/actions/workflows/build.yml)
==========

[![][npm-version]][npm-url] [![][npm-downloads]][npm-url] [![license][license-img]][license-url] [![issues][issues-img]][issues-url] [![stars][stars-img]][stars-url] [![commits][commits-img]][commits-url]

Cross-platform Node.js binding for controlling GPIOs on Silicon Labs CP2102N.

## Installation

```sh
npm install --save cp2102n
```

### Prebuilt addon

This package shiped with prebuilt Node.js native addon for following platforms:

|       | linux | darwin | win32 |
|-------|-------|--------|-------|
| x64   | ✓     | ✓      | TODO  |
| arm   | ✓     | n/a    | n/a   |
| arm64 | ✓     | TODO   |       |

### Build from source

```sh
npm install --save cp2102n --build-from-source
```

Additional build dependencies are required on different platforms:

#### Debian/Ubuntu

```sh
sudo apt install -y build-essential cmake ninja-build pkg-config libusb-1.0-0-dev
```

#### macOS

```sh
brew install cmake ninja pkg-config libusb
```

### Runtime depencencies

#### Debian/Ubuntu

```sh
sudo apt install -y libusb-1.0-0
```

#### macOS

```sh
brew install libusb
```

## Usage

```ts
import { openInterface } from 'cp2102n';

try {
  const device = await openInterface('/dev/cu.usbserial-1140');
  await device.set({ [0]: true, [6]: true });
  device.close();
} catch (e) {
  console.error(e);
}
```

## APIs

### `openInterface(path)`

* `path` - `string`
* Returns: `Promise<CP2102N>`

Open an interface with `path` (i.e. `/dev/cu.usbserialXXX` on macOS, `/dev/ttyUSBX` on Linux). Throws if `path` is not a valid CP2102N device.

### Class: `CP2102N`

#### `cp2102n.set(state)`

* `state` - `{ [pin: number]: boolean }`
* Returns: `Promise<{ [pin: number]: boolean }>`

Set state of pins to high (`true`) or low (`false`). Returns the latest state.

#### `cp2102n.get()`

* Returns: `Promise<{ [pin: number]: boolean }>`

Get state of all PINs.

#### `cp2102n.setRaw(state, mask)`

* `state` - `number`
* `mask` - `number`
* Returns: `Promise<number>`

Set state of masked pins in bits. Returns the latest state bits.

#### `cp2102n.getRaw()`

* Returns: `Promise<number>`

Get state of pins in bits.

#### `cp2102n.close()`

Close interface.

## License

[MIT License](LICENSE)

[npm-version]: https://img.shields.io/npm/v/cp2102n.svg?style=flat-square
[npm-downloads]: https://img.shields.io/npm/dm/cp2102n.svg?style=flat-square
[npm-url]: https://www.npmjs.org/package/cp2102n
[license-img]: https://img.shields.io/github/license/xingrz/node-cp2102n?style=flat-square
[license-url]: LICENSE
[issues-img]: https://img.shields.io/github/issues/xingrz/node-cp2102n?style=flat-square
[issues-url]: https://github.com/xingrz/node-cp2102n/issues
[stars-img]: https://img.shields.io/github/stars/xingrz/node-cp2102n?style=flat-square
[stars-url]: https://github.com/xingrz/node-cp2102n/stargazers
[commits-img]: https://img.shields.io/github/last-commit/xingrz/node-cp2102n?style=flat-square
[commits-url]: https://github.com/xingrz/node-cp2102n/commits/master
