#include "syscall.h"
int main()
{
    // -------Khai báo-------
    int inputID; // id file input
    char c;      // kí tự đọc được
    int n;       // số thời điểm được xét

    int outputID;

    int svID, vnID;

    int f_sv, f_vn; // file sinhvien, voinuoc

    int 

    // ----------------------

    // Đọc số thời điểm được xét
    inputID = Open("input.txt", 1);
    if (inputID == -1)
        return 0;
    n = 0;
    while (c != '\n')
    {
        Read(&c, 1, inputID);
        if (c >= '0' && c <= '9')
            n = n * 10 + ((int)c - '0');
    }
    PrintInt(n);

    // Dùng file output.txt để ghi kết quả
    CreateFile("output.txt");
    outputID = Open("output.txt", 0);
    if (outputID == -1)
        return 0;
    
    // Thực thi chương trình
    svID = Exec("./test/sinhvien");
    if(svID == -1) {
        Close(inputID);
        Close(outputID);
        return 0;
    }
    vnID = Exec("./test/voinuoc");
    if(vnID == -1) {
        Close(inputID);
        Close(outputID);
        return 0;
    }

    // Tạo semaphore

    // Tạo file sinhvien, voinuoc
    f_sv = CreateFile("sinhvien.txt");
    f_vn = CreateFile("voinuoc.txt");

    while(1) {
        while (c != ' ')
        {
            Read(&c, 1, inputID);
            if (c >= '0' && c <= '9')
                n = n * 10 + ((int)c - '0');
        }
    }
}