# S0-counter

S0-counter is a microcontroller project to analyze [S0 impulses][0] that modern
power, water and gas meters can output. It supports multiple channels, and saves
its state in non-volatile FRAM, which makes it persistent across power cycles.

After the signal has been analyzed a parsable string is being output, which can
then be used by other projects, e.g. [volkszaehler][1], in order to get nice
looking graphs and long-term statistics.

**This project is still under development and should be considered non-stable.**

## HIGHLIGHTS

These are some of the highlights, which make this project unique:

- **Multiple channels:** Up to 48 channels are supported, making this the most
  extensive S0 signal analyzer available.

- **Configuration:** Each channel has its own configuration. While the defaults
  should be sane enough in most cases, signal timings and initial values can be
  (re)set at any point in time rather easily by means of a documented
  [UART protocol][2].

- **Open Source**: Both software and hardware are freely available and Open
  Source making it easy and possible to change the project.

## HARDWARE REQUIREMENTS

You can find the schematics in its own [repository][3]. The project is based
upon the AVR family of microcontrollers. Currently the prototype is being
developed for the ATmega328p, but it will be replaced by a bigger brother
later on.

## SOFTWARE REQUIREMENTS

In order to build the source code, a current toolchain for Atmel AVR
microcontrollers is needed. The code itself is tested and developed with the
latest version of avr-gcc, avr-binutils and avr-libc in mind. Other toolchains
or older versions might work just fine, but are not fully tested.

## DOCUMENTATION

The source code itself is documented heavily using [Doxygen][4]. An appropriate
Doxyfile is provided along with the sources and can be found within the `doc/`
directory. It can be used to generate a HTML and PDF reference. Use make's
`doc` target in order to generate the docs from the sources.

## CONFIGURATION

There are various options, which influence the building process and hence the
resulting binary. Various options and debugging aids can be enabled in this
way. The options can be found within `src/config.h`, along with comments about
their actual meaning and possible values.

## BUILDING

There is a Makefile provided with the project. The source code can be simply
build by invoking `make` with the default target.

## FLASHING

The `program` target of the Makefile can be used to flash the resulting binary
to the microcontroller using [avrdude][5].

Make also sure to set the fuse values correctly, i.e.:

- **lfuse**: `0xE2`
- **hfuse**: `0xD9`
- **efuse**: `0xFF`

## CONTRIBUTIONS

The source code is maintained using git. The project along with its [repos][6]
lives over at github.com. Contributions of any kind are highly welcome,
preferably as pull requests. If you are new to git and are not yet familiar
with the process, refer to [this][7] for a detailed description.

In case you are looking for something to work on, you probably want to take a
look at the `TODO` file within the projects root directory or the 
[issue tracker][8] of this project.

## DONATIONS

PayPal: [![PayPal donation](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif "PayPal - The safer, easier way to pay online!")](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=DTKTNRC42GFPN)

Bitcoin: `14kgbYKvtjdH2sNA5iq7jjTmZFNFeWxydo`

## LICENSE

[![GNU GPLv3](http://www.gnu.org/graphics/gplv3-127x51.png "GNU GPLv3")](http://www.gnu.org/licenses/gpl.html)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

[0]: https://de.wikipedia.org/wiki/S0-Schnittstelle
[1]: https://www.volkszaehler.org/
[2]: https://github.com/S0-counter/avr/blob/master/doc/UART_PROTOCOL.md
[3]: https://github.com/S0-counter/avr
[4]: http://www.stack.nl/~dimitri/doxygen/
[5]: http://www.nongnu.org/avrdude/
[6]: https://github.com/S0-counter
[7]: https://help.github.com/articles/using-pull-requests
[8]: https://github.com/S0-counter/avr/issues

