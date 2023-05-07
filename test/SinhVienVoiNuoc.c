// ----------------TIEN TRINH MAIN----------------

#include "syscall.h"
#include "copyright.h"

int main()
{
	// Khai bao
	int isSuccess;							// Bien co dung de kiem tra thanh cong
	OpenFileId fInput, fSinhVien; 		// Bien id cho file
	SpaceId progSV, progVN;					// Bien id cho chuong trinh thuc thi
	int n;									// So luong thoi diem xet
	char c;									// Luu ki tu doc tu file

	// Khoi tao Semaphore
	isSuccess = CreateSemaphore("sinhvien", 0); // Khoi tao semaphore "sinhvien"
	if (isSuccess == -1)						 // khoi tao that bai
		return 0;

	isSuccess = CreateSemaphore("voinuoc", 0); // Khoi tao semaphore "voinuoc"
	if (isSuccess == -1)						// khoi tao that bai
		return 0;

	isSuccess = CreateSemaphore("main", 0); 	// Khoi tao semaphore "main"
	if (isSuccess == -1)						// khoi tao that bai
		return 0;

	// Mo file input.txt de doc
	fInput = Open("input.txt", 1);
	if (fInput == -1) // Mo file that bai
		return 0;

	// Doc so luong thoi diem dang xet
	n = 0;
	while (1)
	{
		Read(&c, 1, fInput); 	// doc tung ki tu trong file input
		if (c != '\n')			// neu chua het 1 dong
		{
			// chuyen doi ki tu (char) doc tu file thanh so nguyen(int)
			if (c >= '0' && c <= '9')
				n = n * 10 + (c - '0');
		}
		else
			break;
	}

	// Thuc thi tien trinh "sinhvien"
	progSV = Exec("./test/sinhvien");
	if (progSV == -1) // thuc thi tien trinh "voinuoc" that bai
	{
		Close(fInput);  // dong file input.txt
		return 0;
	}

	// Thuc thi tien trinh "voinuoc"
	progVN = Exec("./test/voinuoc");
	if (progVN == -1) // thuc thi tien trinh "voinuoc" that bai
	{
		Close(fInput);  // dong file input.txt
		return 0;
	}

	while (n > 0)
	{
		// file sinhvien.txt de ghi tung thoi diem tu file input.txt
		isSuccess = CreateFile("sinhvien.txt");
		if (isSuccess == -1) // tao file that bai
		{
			Close(fInput);  // dong file inpuut.txt
			return 0;
		}

		fSinhVien = Open("sinhvien.txt", 0);
		if (fSinhVien == -1) // mo file that bai
		{
			Close(fInput);  // dong file input.txt
			return 0;
		}

		while (1) // doc va ghi 1 thoi diem
		{
			if (Read(&c, 1, fInput) == -2 || c == '\n') // neu doc toi cuoi file hoac cuoi 1 dong
			{
				Write("/", 1, fSinhVien);	// danh dau cuoi 1 thoi diem
				break;
			}
			Write(&c, 1, fSinhVien);
		}

		Close(fSinhVien);

		Signal("sinhvien");	// Danh thuc tien trinh "sinhvien" de xu ly tung thoi diem

		Wait("main");	// Tien trinh "main" phai cho
		
		n--;
	}

	Close(fInput);  // dong file input.txt

	return 0;
}