# Simple Sniffer

## Description 
This is a simple packet analyser, a 'packet sniffer', developed in C with use of standard libraries along with netinet, linux headers for internet protocols' definitions.

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
- [Features](#features)
- [License](#license)
- [Contacts and Contribution](#contacts-and-contribution)

## Installation 
1. This packet analyser is only compatible with Linux, so you would want to either install Linux normally or install a virtual machine with Linux on it.
   1. Download and install a virtual machine, for example [VirtualBox](https://www.virtualbox.org/wiki/Downloads).
   2. Download an image of Linux, for example [Ubuntu](https://ubuntu.com/download/desktop#support).
   3. Create a virtual machine in VirtualBox where you choose the Ubuntu image you have previously downloaded. 
   4. Run your Linux.
2. Clone the [repository](https://github.com/dakorot/Simple-Sniffer.git).
    1. Install git: 
   ```bash
   sudo apt install git
   ```
   2. Clone the repository:
   ```bash
   git clone https://github.com/dakorot/Simple-Sniffer.git
   ```

## Usage
1. Compile the program:
    ```bash
   gcc main.c
   ```
2. Run the sniffer: 
    ```bash
   sudo ./a.out
   ```
3. Stop whenever you want to by pressing ctrl+C: 
    ```bash
   ^C 
   ```
4. Check the logs.txt file!

## Features
The sniffer is able to capture all the incoming packets but thanks to parsing only TCP, UDP and IP are filtered and written into the logs.txt file. 

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details

## Contacts and Contribution
Any contributions and ideas are very much welcome!

I can be reached any time via [koropenko.daria@gmail.com](mailto:koropenko.daria@gmail.com)

Enjoy Simple Sniffer!
