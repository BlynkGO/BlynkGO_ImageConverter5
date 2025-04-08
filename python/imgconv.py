# >pyinstaller --onefile --add-data "bgimage.exe;." imgconv.py
# โดยมี bgimage.exe วางที่เดียวกับ imgconv.exe ที่ได้ออกมา

#!/usr/bin/python
import subprocess
import argparse
import os
import sys

IMGCONV_VERSION = "V5.0.0"

def main():
    # กำหนด parser เพื่อรับพารามิเตอร์
    parser = argparse.ArgumentParser(prog="imgconv", formatter_class=argparse.RawTextHelpFormatter,
                                     description="BlynkGO ImgConverter " + IMGCONV_VERSION + "\n"
                                                 "===============================\n"
                                                 "  Convert images with BlynkGO ImgConverter\n\n\n"
                                                 "\n\n\n")

    # เพิ่มพารามิเตอร์ -t
    parser.add_argument("-t",  action="store_true", help="Convert with transparency")

    # เพิ่มพารามิเตอร์ -cf
    parser.add_argument("-cf",  choices=["L8", "I1", "I2", "I4", "I8", "A1", "A2", "A4", "A8", "ARGB8888", "XRGB8888", "RGB565", "RGB565A8", "RGB888", "TRUECOLOR", "TRUECOLOR_ALPHA", "AUTO"],
                        help="Color format for the output image\n\n\n")

    # เพิ่มพารามิเตอร์ -o
    parser.add_argument('-o', type=str, help='Output file name')

    # เพิ่มพารามิเตอร์ -of สำหรับ output folder
    parser.add_argument('-of', type=str, help='Output folder for the converted image')

    parser.add_argument("filename", nargs='?', help="Input image file name (bmp, jpg, png)")

    # แปลงพารามิเตอร์ให้เป็น dictionary
    args = parser.parse_args()

    if not args.filename:
        parser.print_help()
        return

    # ตรวจสอบว่าเป็นไฟล์ .exe หรือไม่
    if getattr(sys, 'frozen', False):
        # หากเป็น .exe (ไฟล์ที่ถูกสร้างจาก PyInstaller)
        base_path = sys._MEIPASS
    else:
        # หากยังเป็นไฟล์ Python ธรรมดา
        base_path = os.path.dirname(__file__)

    bgimage_path = os.path.join(base_path, "bgimage.exe")
    
    # ตรวจสอบว่าไฟล์ bgimage.exe มีอยู่จริงหรือไม่
    if not os.path.exists(bgimage_path):
        print(f"ไม่พบไฟล์ bgimage.exe ที่ {bgimage_path}")
        return

    # ตรวจสอบ output directory หาก -of ถูกระบุ
    if args.of:
        output_dir = os.path.abspath(args.of)  # ใช้ folder ที่ระบุใน -of
        if not os.path.exists(output_dir):  # ถ้า directory ไม่มี ให้สร้าง
            os.makedirs(output_dir)
    else:
        output_dir = "."  # ถ้าไม่ได้ระบุ -of ใช้ folder ปัจจุบัน

    # สร้างคำสั่ง
    command = [bgimage_path, args.filename, "--ofmt", "C"]

    if args.t:
        command.extend(["--cf", "RGB565A8"])
    elif args.cf:
        command.extend(["--cf", args.cf])
    else:
        command.extend(["--cf", "RGB565"])

    command.extend(["--output", output_dir])  # ระบุ folder ที่จะเก็บไฟล์ output

    if args.o:
        command.extend(["--outfile", os.path.join(output_dir, args.o)])  # เพิ่มชื่อไฟล์ output

    print("Executing command:", " ".join(command))
    subprocess.run(command)

if __name__ == "__main__":
    main()
