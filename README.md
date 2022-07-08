node-cp2102n [![build](https://github.com/xingrz/node-cp2102n/actions/workflows/build.yml/badge.svg)](https://github.com/xingrz/node-cp2102n/actions/workflows/build.yml)
==========

[![][npm-version]][npm-url] [![][npm-downloads]][npm-url] [![license][license-img]][license-url] [![issues][issues-img]][issues-url] [![stars][stars-img]][stars-url] [![commits][commits-img]][commits-url]

Cross-platform Node.js binding for controlling GPIOs on Silicon Labs CP2102N.

## Installation

```sh
npm install --save cp2102n
```

### Build from source

#### Linux

```sh
sudo apt install -y build-essential cmake ninja-build pkg-config libusb-1.0-0-dev
npm run build
```

#### macOS

```sh
brew info cmake ninja pkg-config libusb
npm run build
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
