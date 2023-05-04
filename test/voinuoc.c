#include "syscall.h"
#include "copyright.h"

int main()
{
    // Khai bao
    int is_Success;                         // bien kiem tra thanh cong
    SpaceId si_fVoiNuoc, si_fResult;        // bien id cho file
    char c;                                 // luu ki tu doc tu file
    int voi1, voi2;                         // voi 1, voi 2
    int dungtichSV;                         // dung tich binh nuoc cua sinh vien
    int isDoneResult;                       // danh dau khi doc xong file result

    voi1 = voi2 = 0;
    // Xu ly voi nuoc
    while(1)
    {
        Wait("sv_vn");                    // cho tien trinh "sv_vn" vao trang thai cho
        // mo file result.txt de ghi voi nuoc nao dang su dung
        si_fResult = Open("result.txt",0); 
        if(si_fResult == -1)            // mo file that bai
        {
            Signal("sinhvien");         // goi tien trinh "sinhvien" hoat dong
            return;
        }

        while(1)
        {
            Wait("voinuoc");            // cho tien trinh "voinuoc" vao trang thai cho
            c = 0;
            // mo file voinuoc.txt de doc dung tich 
            si_fVoiNuoc = Open("voinuoc.txt",1);
            if(si_fVoiNuoc == -1)
            {
                Close(si_fResult);      // dong file result.txt
                Signal("sinhvien");     // goi tien trinh "sinhvien" hoat dong
                return;
            }

            dungtichSV = 0;
            isDoneResult = 0;
            while(1)
            {
                if(Read(&c, 1, si_fVoiNuoc) == -2)       // ??? doc ki tu tu file voinuoc    
                {
                    Close(si_fVoiNuoc);
                    break;
                }
                if(c != '*')                            // 
                {
                    dungtichSV = dungtichSV * 10 + (dungtichSV - 48);
                } 
                else
                {
                    isDoneResult = 1;
                    Close(si_fVoiNuoc);
                    break;
                }
            }
            
            if(dungtichSV !=0)
            {
                if(voi1 <= voi2)        // truong hop thoi gian cua voi 1 it hon (bang) voi 2
                {
                    voi1 = voi1 + dungtichSV;       // day thoi gian vao voi 1
                    Write('1', 1, si_fResult);        // ghi ket qua phan cong cho voi 1
                } else
                {
                    voi2 = voi2 + dungtichSV;       // day thoi gian vao voi 2
                    Write('2',1,si_fResult);        // ghi ket qua phan cong cho voi 2
                }
            }

            if(isDoneResult == 1)
            {
                voi1 = voi2 =0;         // set lai thoi gian ban dau cho 2 voi
                Close(si_fResult);
                Signal("sinhvien");     // goi tien trinh "sinhvien" hoat dong
                break;
            }

            Signal("sinhvien");     // goi tien trinh "sinhvien" hoat dong
        }
    }
}