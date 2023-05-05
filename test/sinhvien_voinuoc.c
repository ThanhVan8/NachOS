#include "syscall.h"
int main()
{
    // -------Khai báo-------
    int f_input; // id file input
    char c;      // kí tự đọc được
    int n;       // số thời điểm được xét

    int f_output;

    int t_sv, t_vn;

    int f_sv, f_vn; // file sinhvien, voinuoc

    int luongNuoc, lenRead;

    int isSuccess;

    // ----------------------

    // Đọc số thời điểm được xét
    f_input = Open("input.txt", 1);
    if (f_input == -1)
        return 0;
    n = 0;
    while (c != '\n')
    {
        Read(&c, 1, f_input);
        if (c >= '0' && c <= '9')
            n = n * 10 + ((int)c - '0');
    }

    // Dùng file output.txt để ghi kết quả
    CreateFile("output.txt");
    f_output = Open("output.txt", 0);
    if (f_output == -1)
        return 0;

    // Tạo semaphore
    isSuccess = CreateSemaphore("sinhvien", 0);
    if(isSuccess == -1)
		return 0;
    isSuccess = CreateSemaphore("voinuoc", 0);
    if(isSuccess == -1)
		return 0;
    isSuccess = CreateSemaphore("main", 0);
    if(isSuccess == -1)
		return 0;

    // Thực thi chương trình
    t_sv = Exec("./test/sinhvien");
    if(t_sv == -1) {
        Close(f_input);
        Close(f_output);
        return 0;
    }
    t_vn = Exec("./test/voinuoc");
    if(t_vn == -1) {
        Close(f_input);
        Close(f_output);
        return 0;
    }
    // Join(t_sv);
    // Join(t_vn);

    // Tạo file sinhvien, voinuoc
    CreateFile("sinhvien.txt");
    CreateFile("voinuoc.txt");

    f_sv = Open("sinhvien.txt", 0);
    if (f_sv == -1)
        return 0;
    f_vn = Open("voinuoc.txt", 0);
    if (f_vn == -1)
        return 0;

    while (n > 0) { // xử lý từng thời điểm
        while (1) // lấy từng dòng dữ liệu input ghi vào sinhvien
        {
            lenRead = Read(&c, 1, f_input);
            if (lenRead < 1 || c == '\n')
                break;
            Write(&c, 1, f_sv);
        }
        Signal("sinhvien"); //1
        Wait("main"); //-1
        n--;
    }
}