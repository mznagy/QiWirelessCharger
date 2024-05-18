# QiWirelessCharger
 Wireless Charger Using ATMEGA32A

This project is a charger controller implemented in C for the Atmega32 microcontroller. It includes functions for digital writing, ADC initialization and reading, byte transmission, and a main function that controls the charger's behavior based on ADC readings.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

You need to have the AVR GCC toolchain installed on your machine. You can install it on macOS using Homebrew:

```bash
brew install avr-gcc
```

### Installing

1. Clone the repository to your local machine:

```bash
git clone https://github.com/username/project.git
```

2. Navigate to the project directory:

```bash
cd project
```

3. Compile the source code:

```bash
avr-gcc -mmcu=atmega32 -o chargerMain.out chargerMain.c
```

4. Upload the compiled code to the Atmega32:

```bash
avrdude -c usbasp -p m32 -U flash:w:chargerMain.out
```

## Built With

* [AVR-GCC](https://gcc.gnu.org/wiki/avr-gcc) - The GCC toolchain for the Atmel AVR microcontrollers

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.


## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

