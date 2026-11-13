# Steganography-Using-C
# 🔐 Image Steganography in C

### 🖼️ Hide Secret Messages Inside Images Using LSB Technique

A command-line based **Image Steganography project developed using C programming** that allows users to securely hide confidential text data inside BMP images and retrieve it whenever required.

This project implements the **Least Significant Bit (LSB) technique**, where secret information is embedded into the least significant bits of image pixel data while maintaining the visual appearance of the original image.

## ✨ Features

- 🔒 **Data Hiding:** Hide secret text files inside BMP images.
- 🔓 **Data Extraction:** Retrieve hidden messages from encoded images.
- 🧩 **LSB Encoding:** Uses the Least Significant Bit technique for data embedding.
- 🖼️ **BMP Image Support:** Works with BMP image files.
- 📄 **Text File Support:** Supports `.txt` files as secret data.
- 🛡️ **Magic String Verification:** Identifies valid steganography-encoded images.
- 📏 **Capacity Checking:** Checks whether the image has sufficient capacity to store secret information.
- 💻 **Command-Line Interface:** Simple command-line operations for encoding and decoding.

## ⚙️ Technologies Used

- 💻 C Programming
- 📂 File Handling
- 🧠 Bitwise Operations
- 🔢 Bit Manipulation
- 🖼️ BMP Image Processing
- 🛠️ GCC Compiler

## 🚀 How to Execute

### 1️⃣ Compile the Project

```bash
gcc *.c -o steganography
```

### 2️⃣ Encoding – Hide Secret Data

```bash
./steganography -e beautiful.bmp secret.txt
```

This embeds the contents of `secret.txt` into `beautiful.bmp` and generates an encoded image named `stego.bmp` by default.

### 3️⃣ Decoding – Extract Secret Data

```bash
./steganography -d stego.bmp
```

This extracts the hidden information and stores it in `output.txt` by default.

## 📁 Project Structure

```text
Image-Steganography/
│
├── main.c
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── common.h
├── types.h
│
├── beautiful.bmp
├── secret.txt
└── README.md
```

## 🧠 Concepts Learned

- Understanding file handling in C.
- Working with binary files.
- Implementing bitwise operators.
- Understanding LSB-based data encoding and decoding.
- Managing file pointers using `fopen()`, `fread()`, `fwrite()`, and `fseek()`.
- Working with structures and modular programming.
- Handling command-line arguments.

## 🎯 Project Objective

The primary objective of this project is to understand how digital information can be hidden inside image files using bit manipulation techniques and how the original secret information can be reconstructed through decoding.

## 👨‍💻 Author

**Manoj Sai**

📌 Embedded Systems & C Programming Enthusiast

---

⭐ If you find this project interesting, don't forget to star the repository!


