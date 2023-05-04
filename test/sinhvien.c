#include "syscall.h"
#include "copyright.h"

int main()
{
    // Khai bao
	int is_Success;                         // bien kiem tra thanh cong
	SpaceId si_fSinhVien, si_fVoiNuoc;	    // bien id cho file
	char c;	                                // bien luu ki tu doc tu file
	int flagVoiNuoc;		                    // bien danh dau nhay den tien trinh "voinuoc"
	int flagMain;		                    // bien danh dau nhay den tien trinh "main"
	int fileLength;		                    // bien luu do dai file
	int pFile;		                        // bien luu con tro trong file

    Signal("sv_vn");	        // chay tien trinh "sv_vn"
    while(1)
    {
        fileLength = 0;
        Wait("sinhvien");

        // tao file result.txt de ghi ket qua phan chia voi nuoc
        is_Success = CreateFile("result.txt");
        if(is_Success == -1)        // tao file that bai
        {
            Signal("main");         // tro ve tien trinh "main"
            return;
        }

        // mo file "sinhvien.txt" de doc du lieu
        si_fSinhVien = Open("sinhvien.txt",1);
        if(si_fSinhVien == -1)          // doc file that bai
        {
            Signal("main");         // tro ve tien trinh "main"
            return;
        }

        fileLength = Seek(-1, si_fSinhVien);        // lay do dai file sinhvien.txt
		Seek(0, si_fSinhVien);                      // dua con tro ve vi tri dau tien cua file sinhvien.txt
		pFile = 0;                                  // danh dau lai vi tri con tro

        // file voinuoc.txt de ghi tung dung tich binh nuoc cua sinh vien
        // tao file voinuoc.txt 
        is_Success = CreateFile("voinuoc.txt");
        if(is_Success == -1)        // tao file that bai
        {
            Close(si_fSinhVien);    // dong file sinhvien.txt
            Signal("main");         // tro ve tien trinh "main"
            return ;
        }

        // mo file voinuoc.txt
        si_fVoiNuoc = Open("voinuoc.txt",0);
        if(si_fVoiNuoc == -1)        // mo file that bai
        {
            Close(si_fSinhVien);    // dong file sinhvien.txt
            Signal("main");         // tro ve tien trinh "main"
            return;
        }

        // ghi dung tich vao voinuoc.txt tu sinhvien.txt
        while(pFile < fileLength)
        {
            flagVoiNuoc = 0;
            
            Read(&c, 1, si_fSinhVien);          // doc ki tu tu file sinhvien.txt
            
            if(c != ' ')                    
            {
                Write(&c,1, si_fVoiNuoc);       // ghi vao file voinuoc.txt dung tich binh nuoc
            } else 
                flagVoiNuoc = 1;

            if(pFile == fileLength -1)
            {
                Write('*',1,si_fVoiNuoc);
                flagVoiNuoc = 1;
            }

            if(flagVoiNuoc == 1)
            {
                Close(si_fVoiNuoc);             // dong file voinuoc.txt
                Signal("voinuoc");              // goi tien trinh "voinuoc" hoat dong
                Wait("sinhvien");               // cho tien trinh "sinhvien" cho de "voinuoc" hoat dong


                // tao mot file voinuoc.txt khac va thuc hien nhu tren
                is_Success = CreateFile("voinuoc.txt");
				if(is_Success == -1)
				{
					Close(si_fSinhVien);
					Signal("main"); // tro ve tien trinh chinh
					return;
				}
		

				// Mo file voinuoc.txt de ghi tung dung tich nuoc cua sinhvien
				si_fVoiNuoc = Open("voinuoc.txt", 0);
				if(si_fVoiNuoc == -1)
				{
					Close(si_fSinhVien);
					Signal("main"); // tro ve tien trinh chinh
					return;
				}
            }
            pFile++;    // dua con tro den vi tri tiep theo

        }
        Signal("main");
    }

}