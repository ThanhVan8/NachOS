// ----------------TIEN TRINH VOINUOC----------------

#include "syscall.h"
#include "copyright.h"

int main()
{
    // Khai bao
    int isSuccess;                         // bien kiem tra thanh cong
    OpenFileId fVoiNuoc, fOutput;          // bien id cho file
    char c;                                // luu ki tu doc tu file
    int voi1, voi2;                        // voi 1, voi 2
    int dungtichSV;                        // dung tich binh nuoc cua sinh vien

    // file output.txt dung de ghi ket qua rot nuoc
	isSuccess = CreateFile("output.txt");
	if (isSuccess == -1) // tao file that bai
    {
        Signal("sinhvien"); // goi tien trinh "sinhvien" hoat dong
		return 0;
    }
    fOutput = Open("output.txt", 0);
    if (fOutput == -1) // mo file that bai
    {
        Signal("sinhvien"); // goi tien trinh "sinhvien" hoat dong
        return 0;
    }

    voi1 = voi2 = 0; // dat thoi gian ban dau la 0

    while (1)   // xu ly phan chia voi nuoc cho tung sinh vien
    {
        Wait("voinuoc"); // cho tien trinh "sinhvien" ghi 1 dung tich

        // file voinuoc.txt de doc dung tich
        fVoiNuoc = Open("voinuoc.txt", 1);
        if (fVoiNuoc == -1)
        {
            Close(fOutput); // dong file voinuoc.txt
            Signal("sinhvien"); // goi tien trinh "sinhvien" hoat dong
            return 0;
        }

        dungtichSV = 0;

        while (1)
        {
            Read(&c, 1, fVoiNuoc);
            if (c == ' ' || c == '/')    // neu doc het 1 dung tich
            {
                Close(fVoiNuoc);    // dong file voinuoc.txt
                break;
            }
            else if (c >= '0' && c <= '9')
            {
                Write(&c, 1, fOutput);     // ghi dung tich binh dang xet vao file output.txt
                dungtichSV = dungtichSV * 10 + (c - '0');
            }
        }

        if (dungtichSV != 0)
        {
            if (voi1 <= voi2) // truong hop thoi gian cua voi 1 it hon (bang) voi 2
            {
                voi1 += dungtichSV;         // day thoi gian vao voi 1
                Write(" 1  ", 4, fOutput);    // ghi ket qua phan cong cho voi 1
            }
            else
            {
                voi2 += dungtichSV;         // day thoi gian vao voi 2
                Write(" 2  ", 4, fOutput);    // ghi ket qua phan cong cho voi 2
            }
        }

        if (c == '/') // neu la binh nuoc cuoi cua 1 thoi diem
        {
            voi1 = voi2 = 0; // set lai thoi gian ban dau cho 2 voi
            Write("\r\n", 2, fOutput);
        }

        Signal("sinhvien"); // goi tien trinh "sinhvien" de ghi dung tich tiep theo
    }
}