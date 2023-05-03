#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 32


int main()
{
	// Khai bao
	int is_Success;             // Bien co dung de kiem tra thanh cong
	SpaceId si_fInput, si_fOutput, si_fSinhVien, si_fResult;	    // Bien id cho file
	int n;	                    // So luong thoi diem xet
	char c;	                    // Luu ki tu doc tu file


	// Khoi tao Semaphore 
	is_Success = CreateSemaphore("sinhvien", 0);		// Khoi tao semaphore "sinhvien"
	if(is_Success == -1)        // khoi tao that bai
		return 1;

	is_Success = CreateSemaphore("voinuoc", 0);			// Khoi tao semaphore "voinuoc"
	if(is_Success == -1)
		return 1;

    is_Success = CreateSemaphore("main", 0);			// Khoi tao semaphore "main"
	if(is_Success == -1)
		return 1;

	is_Success = CreateSemaphore("pre", 0);
	if(is_Success == -1)
		return 1;

    // Mo - doc file input.txt 
    si_fInput = Open("input.txt",1);
    if(si_fInput == -1)     // Mo file that bai
        return -1;
    
    // Tao file output 
    is_Success = CreateFile("output.txt");
    if(is_Success == -1)        // tao file that bai
        return 1;

    // Mo file output de doc va ghi ket qua
    si_fOutput = Open("output.txt", 0);
    if(si_fOutput == -1)        // mo file that bai
    {
        Close(si_fInput);       // dong file input
        return -1;
    }        

	// Doc so luong thoi diem dang xet
	n = 0;
	while(1)
	{
		Read(&c, 1, si_fInput); 		// doc tung ki tu trong file input
		if(c != '\n')					// neu khong la ki tu xuong dong
		{
			// chuyen doi ki tu (char) doc tu file thanh so nguyen(int)
			if(c >= '0' && c <= '9')
				n = n * 10 + (c - 48); 
		}
		else
			break;						// ki tu doc vao khong la 1 so
	}

	// Thuc thi tien trinh "sinhvien"
	is_Success = Exec("./test/sinhvien");
	if(is_Success == -1)		// thuc thi tien trinh "voinuoc" that bai
	{
		Close(si_fInput);		// dong file input.txt
		Close(si_fOutput);		// dong file output.txt
		return 1;
	}

	// Goi thuc thi tien trinh voinuoc
	is_Success = Exec("./test/voinuoc");
	if(is_Success == -1)		// thuc thi tien trinh "voinuoc" that bai
	{
		Close(si_fInput);		// dong file input.txt
		Close(si_fOutput);		// dong file output.txt
		return 1;
	}

	// Xu ly den khi het thoi diem dang xet
	while(n--)
	{
		// Tao file sinhvien.txt
		is_Success = CreateFile("sinhvien.txt");
		if(is_Success == -1)		// tao file that bai
		{
			Close(si_fInput);		// dong file inpuut.txt
			Close(si_fOutput);		// dong file output.txt
			return 1;
		}
		
		// Mo file sinhvien.txt (de ghi tung dong tu file input.txt)
		si_fSinhVien = Open("sinhvien.txt", 0);
		if(si_fSinhVien == -1)		// mo file that bai
		{
			Close(si_fInput);		// dong file input.txt
			Close(si_fOutput);		// dong file output.txt
			return 1;
		}

		while(1)
		{
			if(Read(&c, 1, si_fInput) <1) 		// doc noi dung trong file input
			{
				// Doc toi cuoi file
				break;
			}
			if(c != '\n') 						// kiem soat ki tu xuong dong de thoat vong lap
			{
				Write(&c, 1, si_fSinhVien);		// ghi vao file sinhvien.txt			
			}
			else
				break;							// khi doc het 1 doc
						
		}

		// Dong file sinhvien.txt
		Close(si_fSinhVien);
			
		// Goi tien trinh sinhvien hoat dong
		Signal("sinhvien");

		// Tien trinh "main" phai cho
		Wait("main");	
		
		// Thuc hien doc file tu result va ghi ket qua vao file output.txt
		// file result.txt de ghi thu tu voi nuoc tuong ung voi sinh vien den rot nuoc - duoc tao trong sinhvien.c
		si_fResult = Open("result.txt", 1);		
		if(si_fResult == -1)		// mo file that bai
		{
			Close(si_fInput);		// dong file input.txt
			Close(si_fOutput);		// dong file output.txt
			return 1;
		}

		PrintString("\n Lan thu: ");
		PrintInt(n);
		PrintString("\n");	

		// Doc cac voi vao output.txt		
		while(1)
		{
			if(Read(&c, 1, si_fResult) <1)		// kiem tra het 1 dong chua
			{
				Write("\r\n", 2, si_fOutput);	
				Close(si_fResult);			// dong file result.txt
				Signal("pre");				
				break;
			}
			Write(&c, 1, si_fOutput);		// ghi voi nuoc tuong ung vao file output.txt
			Write(" ", 1, si_fOutput);		// cac voi nuoc cach nhau 1 khoang " "
			
		}
		
	
	Close(si_fInput);		// dong file input.txt
	Close(si_fOutput);		// dong file output.txt
	return 0;
}

	
