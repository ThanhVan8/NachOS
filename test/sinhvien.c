// ----------------TIEN TRINH SINHVIEN----------------

#include "syscall.h"
#include "copyright.h"

int main()
{
    // Khai bao
    int isSuccess;                     // bien kiem tra thanh cong
    OpenFileId fSinhVien, fVoiNuoc;     // bien id cho file
    char c;                             // bien luu ki tu doc tu file

    while (1) // xu ly tung thoi diem
    {
        
        Wait("sinhvien");   // cho tien trinh "main" ghi cac dung tich cua 1 thoi diem 
                            // vao file sinhvien.txt

        // file voinuoc.txt de ghi tung dung tich binh nuoc cua sinh vien
        isSuccess = CreateFile("voinuoc.txt");
        if (isSuccess == -1) // tao file that bai
        {
            Signal("main");   // danh thuc tien trinh "main"
            return 0;
        }
        
        // mo file "sinhvien.txt" de doc cac dung tich
        fSinhVien = Open("sinhvien.txt", 1);
        if (fSinhVien == -1)    // doc file that bai
        {
            Signal("main");     // danh thuc tien trinh "main"
            return 0;
        }

        while (1)
        {
            fVoiNuoc = Open("voinuoc.txt", 0);
            if (fVoiNuoc == -1) // mo file that bai
            {
                Close(fSinhVien); // dong file sinhvien.txt
                Signal("main");   // danh thuc tien trinh "main"
                return 0;
            }

            while (1)   // doc tung dung tich binh nuoc
            {
                Read(&c, 1, fSinhVien); // doc ki tu tu file sinhvien.txt
                Write(&c, 1, fVoiNuoc); // ghi vao file voinuoc.txt dung tich binh nuoc
                if (c == ' ' || c == '/') // neu doc het 1 dung tich hoac doc toi dung tich cuoi
                    break;
            }

            Close(fVoiNuoc);    // dong file voinuoc.txt

            Signal("voinuoc");  // goi tien trinh "voinuoc" hoat dong
            Wait("sinhvien");   // cho tien trinh "sinhvien" cho de "voinuoc" hoat dong

            if (c == '/') // neu doc toi dung tich cuoi
                break;
        }

        Close(fSinhVien); // dong file sinhvien.txt

        Signal("main"); // danh thuc tien trinh MAIN de doc thoi diem tiep theo
    }
}