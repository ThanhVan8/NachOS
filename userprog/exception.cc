// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2 -- đọc tại thanh ghi 2
//		arg1 -- r4 -- tham số thứ nhất sẽ bắt đầu lưu tại thanh ghi 4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever! -- phải tăng program counter lên
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

#define MaxBuffer 255
#define MaxFileLength 32
#define MaxFile 10

char *User2System(int virtAddr, int limit)
{
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; // need for terminal string
    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

int System2User(int virtAddr, int len, char *buffer)
{
    if (len < 0)
        return -1;
    if (len == 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

// tang program counter
void increasePC() {
    // Compute next pc
    int pcAfter = machine->ReadRegister(NextPCReg) + 4;
    // Advance program counters.
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, pcAfter);
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2); // đọc tại thanh ghi 2

    switch(which) {
        case NoException:
            return;
        case PageFaultException:
        {
            DEBUG('a', "\n No valid translation found.");
            printf("\n \n No valid translation found.");
            interrupt->Halt();
            return;
        }
        case ReadOnlyException:
        {
            DEBUG('a', "\n Write attempted to page marked read-only.");
            printf("\n\n Write attempted to page marked read-only.");
            interrupt->Halt();
            return;
        }
        case BusErrorException:
        {
        DEBUG('a', "\n Translation resulted in an invalid physical address.");
        printf("\n\n Translation resulted in an invalid physical address.");
        interrupt->Halt();
        return;
        }
        case AddressErrorException:
        {
            DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space.");
            printf("\n\n Unaligned reference or one that was beyond the end of the address space.");
            interrupt->Halt();
            return;
        }
        case OverflowException:
        {
            DEBUG('a', "\n Integer overflow in add or sub.");
            printf("\n\n Integer overflow in add or sub.");
            interrupt->Halt();
            return;
        }
        case IllegalInstrException:
        {
            DEBUG('a', "\n Unimplemented or reserved instr.");
            printf("\n\n Unimplemented or reserved instr.");
            interrupt->Halt();
            return;
        }
        case NumExceptionTypes:
        {
            DEBUG('a', "\n Num Exception Types.");
            printf("\n\n Num Exception Types.");
            interrupt->Halt();
            return;
        }
        case SyscallException:
        {
            switch(type) {
                case SC_Halt:
                {
                    DEBUG('a', "\nShutdown, initiated by user program.");
                    printf("\n\nShutdown, initiated by user program.");
                    interrupt->Halt();
                    return;
                }
                case SC_ReadInt:
                {
                    char* buff = new char[MaxBuffer + 1];
                    int len = gSynchConsole->Read(buff, MaxBuffer); // doc vao chuoi user nhap,
                                                                    // tra ve so byte doc duoc
                    buff[len] = '\0';

                    // kiem tra so am
                    int sign = 1; // so duong
                    if(buff[0] == '-') {
                        buff[0] = '0';
                        sign = -1; // so am
                    }

                    int res = 0; // ket qua so nguyen
                    for(int i = 0; i < len; i++) {
                        if(buff[i] < '0' || buff[i] > '9') { // neu khong phai la so
                            DEBUG('a', "\nInvalid input.");
                            printf("\n Invalid input.");
                            delete buff;
                            machine->WriteRegister(2, 0); // tra ve 0
                            increasePC();
                            return;
                        }
                        // neu la so
                        // chuyen char thanh int -> gan vao ket qua
                        res = res * 10 + sign * (buff[i] - '0');
                    }

                    machine->WriteRegister(2, res); // tra so nguyen doc duoc
                    delete buff;
                    break;
                }
                case SC_PrintInt:
                {
                    int number = machine->ReadRegister(4); // so nguyen duoc truyen vao
                    // neu la so 0
                    if(number == 0) {
                        gSynchConsole->Write("0", 1); // xuat so 0
                        increasePC();
                        return;
                    }

                    char* buff = new char[MaxBuffer + 1]; // chuoi ki tu dung de in so nguyen
                    
                    bool isNeg = false; // bien kiem tra co phai la so am
                    if(number < 0) { // neu la so am
                        isNeg = true;
                        number *= -1; // chuyen thanh so duong
                    }

                    int len = 0; // do dai chuoi ki tu so
                    // dem so chu so
                    int temp = number;
                    while(temp > 0) {
                        len++;
                        temp /= 10;
                    }

                    for(int i = len - 1; i >= 0; i--) {
                        buff[i] = (char)((number % 10) + '0'); // chuyen int thanh char
                        number /= 10;
                    }
                    buff[len] = '\0';

                    if(isNeg) // neu la so am
                        gSynchConsole->Write("-", 1); // xuat them dau -
                    gSynchConsole->Write(buff, len + 1); // xuat ket qua ra man hinh console

                    delete buff;
                    break;
                }
                case SC_ReadChar:
                {
                    char *buff = new char [MaxBuffer + 1];
                    int nBytes = gSynchConsole->Read(buff, MaxBuffer); // tra ve so luong byte ma nguoi dung su dung 

                    // So ki tu nhap vao nhieu hon 1 - nhap chuoi
                    if(nBytes > 1) 
                    {
                        printf("\nInvalid - Number of characters is greater than 1");
                        DEBUG('a', "\nError: Number of characters is greater than 1");
                        machine->WriteRegister(2, 0);  // tra ve 0 
                    } 
                    else if(nBytes == 0) // kí tự rỗng - null
                    {
                        printf("\nInvalid - Don't have any character") ;
                        DEBUG('a',"\nError: Don't have any character");
                        machine->WriteRegister(2, 0);  // tra ve 0
                    }
                    else // ki tu hop le
                    {
                        char c = *buff; // ki tu doc duoc luu tai vi tri dau tien cua mang buff
                        machine->WriteRegister(2, c); // tra gia tri ki tu doc duoc vao thanh ghi 2
                    }
                    delete buff;
                    break;
                }
                case SC_PrintChar:
                {
                    char c = (char)machine->ReadRegister(4); // lay gia tri tai thanh ghi 4
                    gSynchConsole->Write(&c, 1); // in ra man hinh console gia tri cua c, kich thuoc 1Byte
                    break;
                }
                case SC_ReadString:
                {
                    int buffAddr = machine->ReadRegister(4); // dia chi cua *buffer
                    int maxLen = machine->ReadRegister(5); // do dai toi da cua chuoi
                    
                    char *buffer = new char[maxLen + 1];
                    int len = gSynchConsole->Read(buffer, maxLen);   // doc chuoi user nhap,
                                                                        // tra ve do dai chuoi doc duoc
                    buffer[len] = '\0';

                    System2User(buffAddr, len + 1, buffer);  // chuyen du lieu tu kernelspcae qua userspace
                    delete buffer;
                    break;
                }
                case SC_PrintString:
                {
                    int buffAddr = machine->ReadRegister(4);    // dia chi cua *buffer
                    char* buffer = User2System(buffAddr, MaxBuffer);    // chuyen du lieu tu userspace qua kernelspcae 

                    int len = strlen(buffer);

                    gSynchConsole->Write(buffer, len + 1); // xuat chuoi ra console
                    delete buffer;
                    break;
                }
                case SC_CreateFile:
                {
                    //tao file rong, tra ve 0 neu thanh cong, -1 neu co loi
                    int virtAddr;
                    char* filename;
                    
                    virtAddr = machine->ReadRegister(4); 
                    filename = User2System(virtAddr, MaxFileLength + 1); // chuyen tu user qua kernel

                    if (strlen(filename) == 0)
                    {
                        printf("\nFile name is not valid!");
                        machine->WriteRegister(2, -1); 
                        delete filename;
                        break;
                    }

                    if (filename == NULL)  //khong doc duoc file
                    {
                        printf("\nNot enough memory in system");
                        machine->WriteRegister(2, -1); 
                        delete filename;
                        break;
                    }

                    if (!fileSystem->Create(filename, 0)) //tao file bang filesystem
                    {
                        printf("\nError create file '%s'", filename);// tao file khong thanh cong
                        machine->WriteRegister(2, -1);
                        delete filename;
                        break;
                    }

                    printf("\nCreate file '%s' success", filename);//tao file thanh cong
                    machine->WriteRegister(2, 0);
                    delete filename;
                    break;
                }

                case SC_Open:
                {
                    int bufAddr = machine->ReadRegister(4); 
                    int type = machine->ReadRegister(5);
                    
                    // Khi mo stdin hay stdout, khong tang slg files
                    char *buf = User2System(bufAddr, MaxFileLength + 1);
                    if (strcmp(buf, "stdin") == 0)
                    {
                        //printf("Stdin mode\n");
                        machine->WriteRegister(2, 0); // trả về id file là 0
                        delete buf;
                        break;
                    }
                    if (strcmp(buf, "stdout") == 0)
                    {
                        printf("\nStdout mode");
                        machine->WriteRegister(2, 1); // trả về id file là 1
                        delete buf;
                        break;
                    }

                    int FreeSlot = fileSystem->FindFreeSlot();
                    fileSystem->fTab[FreeSlot] = fileSystem->Open(buf, type);
                    if (fileSystem->fTab[FreeSlot] != NULL)
                    {
                        printf("\nOpen file '%s' success", buf);
                        machine->WriteRegister(2, FreeSlot); // trả về id file
                    }
                    else 
                    {
                        printf("\nCan not open file '%s'", buf);
                        machine->WriteRegister(2, -1);
                    }
                    delete buf;
                    break;
                }

                case SC_Close:
                {
                    int no = machine->ReadRegister(4);

                    if(no < 0 || no > MaxFile - 1) { // neu id file nam ngoai bang mo ta file
                        machine->WriteRegister(2, -1);
                        printf("\nNot in file table!");
                        break;
                    }
                    if (fileSystem->fTab[no])   // neu file ton tai
                    {
                        delete fileSystem->fTab[no]; // xoa file khoi bang mo ta file
                        fileSystem->fTab[no] = NULL;
                        machine->WriteRegister(2, 0);
                        printf("\nClose file success");
                        break;
                    }
                    else
                    {
                        machine->WriteRegister(2, -1);
                        printf("\nClose file failed");
                        break;
                    }
                }

                case SC_Read:
                {
                    int virtAddr = machine->ReadRegister(4);
                    int charcount = machine->ReadRegister(5);
                    int openf_id = machine->ReadRegister(6);

                    if ( openf_id > MaxFile - 1 || openf_id < 0 || openf_id == 1) // neu id cua file nam ngoai bang mo ta file
                    {                                                             // hoac doc tu stdout
                        printf("\nTry to open invalid file id '%d'", openf_id);
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    if (fileSystem->fTab[openf_id] == NULL) // neu file khong ton tai
                    {
                        printf("\nFile id '%d' not exist!", openf_id);
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    char *buf = User2System(virtAddr, charcount);
                    if (openf_id == 0) // neu doc tu console
                    {
                        int sz = gSynchConsole->Read(buf, charcount);
                        System2User(virtAddr, sz, buf);
                        machine->WriteRegister(2, sz);  // tra ve so ki tu doc duoc

                        delete buf;
                        break;
                    }

                    int len = fileSystem->fTab[openf_id]->Read(buf, charcount);
                    if (len > 0)
                    {
                        // chuyen du lieu tu kernel sang user
                        System2User(virtAddr, len, buf);
                        machine->WriteRegister(2, len); // tra ve so ki tu doc duoc
                    }
                    else    // neu cham toi cuoi file
                        machine->WriteRegister(2, -2);

                    delete buf;
                    break;
                }

                case SC_Write:
                {
                    int virtAddr = machine->ReadRegister(4);
                    int charcount = machine->ReadRegister(5);
                    int openf_id = machine->ReadRegister(6);

                    if (openf_id > MaxFile - 1 || openf_id < 0 || openf_id == 0)    // neu id cua file nam ngoai bang mo ta file
                    {                                                               // hoac ghi vao stdin
                        printf("\nCannot write to invalid file id '%d'", openf_id);                                                
                        machine->WriteRegister(2, -1);
                        break;
                    }
                    
                    if (fileSystem->fTab[openf_id] == NULL) // neu file khong ton tai
                    {
                        printf("\nFile id '%d' not exist!", openf_id);
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    // file chi doc	
                    if (fileSystem->fTab[openf_id]->type == 1)
                    {
                        printf("\nTry to modify read-only file id '%d'", openf_id);
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    // ghi ra console
                    char *buf = User2System(virtAddr, charcount);
                    if (openf_id == 1)
                    {
                        int i = 0;
                        do
                        {
                            gSynchConsole->Write(buf + i, 1);
                            i++;
                        }  while (buf[i] != '\0');

                        machine->WriteRegister(2, i); // tra ve so ki tu ghi duoc
                        delete buf;
                        break;
                    }

                    // ghi vao file
                    int len = fileSystem->fTab[openf_id]->Write(buf, charcount);
                    if (len > 0)
                    {
                        machine->WriteRegister(2, len); // tra ve so ki tu ghi duoc
                        delete buf;
                        break;
                    }
                    delete buf;
                    break;
                }

                case SC_Exec:
                {
                    int virtAddr = machine->ReadRegister(4);	// doc dia chi ten chuong trinh tu thanh ghi r4
                    char *name = User2System(virtAddr, MaxFileLength + 1); // Lay ten chuong trinh, nap vao kernel space

                    if (name == NULL)
                    {
                        DEBUG('a', "\nCan't open file '%s'", name);
                        printf("\nCan't open file '%s'", name);
                        machine->WriteRegister(2, -1);// tra ve -1
                        break;
                    }

                    int pid = pTab->ExecUpdate(name);
                    machine->WriteRegister(2, pid); // tra ve id cua tien trinh

                    delete name;
                    break;
                }
                case SC_Join:
                {
                    int id = machine->ReadRegister(4); // Đọc id của tiến trình cần Join từ thanh ghi r4
                    int ec = pTab->JoinUpdate(id);
                    machine->WriteRegister(2, ec);
                    break;
                }
                case SC_Exit:
                {
                    int exitStatus = machine->ReadRegister(4); // Đọc exitStatus từ thanh ghi r4
                    int ec = pTab->ExitUpdate(exitStatus);
                    machine->WriteRegister(2, ec);
                    break;
                }
                case SC_CreateSemaphore:
                {
                    // Đọc địa chỉ “name” từ thanh ghi r4.
                    int nameAddr = machine->ReadRegister(4);
                    // Đọc giá trị “semval” từ thanh ghi r5.
                    int semval = machine->ReadRegister(5);
                    // Tên địa chỉ “name” lúc này đang ở trong user space. Gọi hàm User2System đã được khai báo trong 
                    // lớp machine để chuyển vùng nhớ user space tới vùng nhớ system space.
                    char *name = User2System(nameAddr, MaxFileLength + 1);
                    if (strlen(name) == 0)
                    {
                        printf("\nFile name is not valid");
                        machine->WriteRegister(2, -1); 
                        delete name;
                        break;
                    }
                    if (name == NULL)  //khong doc duoc file
                    {
                        printf("\nNot enough memory in system");
                        machine->WriteRegister(2, -1); 
                        delete name;
                        break;
                    }
                    // Gọi thực hiện hàm semTab->Create(name,semval) để tạo Semaphore, nếu có lỗi thì báo lỗi.
                    int create = semTab->Create(name, semval);
                    if (create == -1) {
                        DEBUG('a', "\nCannot create semaphore %s!", name);
                        printf("\nCannot create semaphore %s!", name);
                        machine->WriteRegister(2, -1); 
                        delete name;
                        break;
                    }
                    // Lưu kết quả thực hiện vào thanh ghi r2.
                    machine->WriteRegister(2, 0); 
                    delete name;
                    break;
                }
                case SC_Wait:
                {
                    // Đọc địa chỉ “name” từ thanh ghi r4.
                    int nameAddr = machine->ReadRegister(4);
                    char *name = User2System(nameAddr, MaxFileLength + 1);

                    // kiểm tra bộ nhớ 
                    if(name == NULL)
                    {
                        DEBUG('a', "\nNot enough memory in system");
                        printf("\nNot enough memory in system");
                        machine->WriteRegister(2,-1);
                        delete name;
                        break;
                    }

                    int check = semTab->Wait(name);

                    // không tìm thấy semaphore "name" trong sTab
                    if(check == -1)
                    {
                        DEBUG('a', "\nThis semaphore not exist");
                        printf("\nThis semaphore not exist");
                        machine->WriteRegister(2, -1);
                        delete name;
                        break;
                    }

                    machine->WriteRegister(2, 0);
                    delete name;
                    break;           
                }
                case SC_Signal:
                {
                    // Đọc địa chỉ “name” từ thanh ghi r4.
                    int nameAddr = machine->ReadRegister(4);
                    char *name = User2System(nameAddr, MaxFileLength + 1);

                    // kiểm tra bộ nhớ 
                    if(name == NULL)
                    {
                        DEBUG('a', "\nNot enough memory in system");
                        printf("\nNot enough memory in system");
                        machine->WriteRegister(2, -1);
                        delete name;
                        break;
                    }

                    int check = semTab->Signal(name);

                    // không tìm thấy semaphore "name" trong sTab
                    if(check == -1)
                    {
                        DEBUG('a', "\nThis semaphore not exist");
                        printf("\nThis semaphore not exist");
                        machine->WriteRegister(2, -1);
                        delete name;
                        break;
                    }

                    machine->WriteRegister(2, 0);
                    delete name;
                    break;           
                }
                default:
                {
                    printf("\n Unexpected user mode exception (%d %d)", which, type);
                    interrupt->Halt(); 
                    break;
                }
            }
        }
        increasePC(); // tang program counter
    }
}
