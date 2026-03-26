# 🖼️ LSB Image Steganography in C

## 🔍 Overview

LSB (Least Significant Bit) Image Steganography is a technique used to hide secret data inside an image without noticeably altering its appearance. This project implements steganography using C, where a secret message or file is embedded into an image and can later be extracted.

---

## 🎯 Objective

The objective of this project is to understand data hiding techniques and apply bit-level manipulation to securely embed and retrieve information from images.

---

## ✨ Features

* 🔐 Encode (hide) secret data into an image
* 🔍 Decode (extract) hidden data from the image
* 🧠 Uses Least Significant Bit (LSB) technique
* 📁 Supports embedding text data inside BMP images
* 🛡️ Maintains image quality after encoding
* ⚡ Efficient bit-level operations

---

## 🛠️ Technologies Used

* **Language:** C
* **Concepts:**

  * Bit Manipulation
  * File Handling
  * Binary Data Processing
  * Pointers
  * Structures

---

## 🧠 Key Concepts Implemented

* Manipulation of least significant bits to hide data
* Reading and writing binary image files (BMP format)
* Encoding and decoding algorithms
* Data integrity and retrieval process
* Memory management using pointers

---

## ⚙️ Working Principle

* Each pixel in an image is made up of bytes
* The least significant bit (LSB) of each byte is modified
* Secret data is converted into binary form
* Bits of secret data are embedded into image bytes
* During decoding, these bits are extracted to reconstruct the original data

---

## 📁 Project Structure
Steganography/

│── encode.c        // Encoding logic to hide data in image  
│── encode.h        // Function declarations for encoding  
│── decode.c        // Decoding logic to extract hidden data  
│── decode.h        // Function declarations for decoding  
│── types.h         // Data types and structures used  
│── common.h        // Contains common definitions and magic string for validation  
│── main.c          // Main driver code  

---

## ▶️ How to Run

1. Compile the program:

   ```
   gcc *.c
   ```

2. Run encoding:

   ```
   ./a.out -e beautiful.bmp secret.txt stego.bmp
   ```

3. Run decoding:

   ```
   ./a.out -d stego.bmp decode.txt
   ```

---

## 🎓 Training

This project was developed during training at **Emertxe Information Technologies**, where concepts like file handling, bit manipulation, and real-time problem solving were applied.

---

## 🚀 Future Enhancements

* Support for multiple image formats (PNG, JPG)
* Add password-based encryption
* Improve security using advanced algorithms
* GUI-based interface

---

## 👩‍💻 Author

**Samiksha Mane**
B.Tech Electronics & Telecommunication Engineering

---

## 📎 Conclusion

This project demonstrates how data can be securely hidden within digital media using low-level programming techniques. It enhances understanding of bit manipulation, file operations, and security concepts, making it a strong system-level programming project.
