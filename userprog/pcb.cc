#include "pcb.h"
#include "utility.h"
#include "system.h"
#include "thread.h"
#include "addrspace.h"

extern void StartProcess_2(int id);

PCB::PCB()
{
	this->parentID = -1;

	this->numwait = this->exitcode = 0;
	this->thread = NULL;

	this->joinsem = new Semaphore("joinsem", 0);
	this->exitsem = new Semaphore("exitsem", 0);
	this->multex = new Semaphore("multex", 1);
}
PCB::PCB(int id)
{
	if (id == 0)
		this->parentID = -1;
	else
		this->parentID = currentThread->processID;

	this->numwait = this->exitcode = 0;
	this->thread = NULL;

	this->joinsem = new Semaphore("joinsem", 0);
	this->exitsem = new Semaphore("exitsem", 0);
	this->multex = new Semaphore("multex", 1);
}
PCB::~PCB()
{
	if(joinsem)
		delete this->joinsem;
	if(exitsem)
		delete this->exitsem;
	if(multex)
		delete this->multex;
	if(thread)
	{		
		thread->FreeSpace();
		thread->Finish();
	}
}

// tạo một thread moi có tên là filename và processID là pid
int PCB::Exec(char* filename, int id)
{  
    // Gọi mutex->P(); để giúp tránh tình trạng nạp 2 tiến trình cùng 1 lúc.
	multex->P();
    // Kiểm tra thread đã khởi tạo thành công chưa, nếu chưa thì báo lỗi là không đủ bộ nhớ, gọi mutex->V() và return.             
	this->thread = new Thread(filename);
	if(this->thread == NULL){
		DEBUG('a', "\nNot enough memory to create a thread!");
		printf("\nNot enough memory to create a thread!");
        multex->V();
		return -1;
	}
	//  Đặt processID của thread này là id.
	this->thread->processID = id;
	// Đặt parrentID của thread này là processID của thread gọi thực thi Exec
	this->parentID = currentThread->processID;
	// Gọi thực thi Fork(StartProcess_2,id) => Ta cast thread thành kiểu int, sau đó khi xử lý hàm StartProcess ta cast Thread về đúng kiểu của nó.
 	this->thread->Fork(StartProcess_2, id);
    multex->V();
	// Trả về id.
	return id;
}

// Trả về ProcessID của tiến trình gọi thực hiện
int PCB::GetID() {
	return this->thread->processID;
}

// Trả về số lượng tiến trình chờ
int PCB::GetNumWait() {
	return this->numwait;
}

// Tiến trình cha đợi tiến trình con kết thúc
void PCB::JoinWait()
{
	// Gọi joinsem->P() để tiến trình chuyển sang trạng thái block và ngừng lại, 
	// chờ JoinRelease để thực hiện tiếp.
    joinsem->P();
}

// Cho phép tiến trình con kết thúc
void PCB::ExitRelease() 
{
	// Gọi exitsem-->V() để giải phóng tiến trình đang chờ. 
    exitsem->V();
}

// Báo cho tiến trình cha thực thi tiếp
void PCB::JoinRelease()
{ 
	// Gọi joinsem->V() để giải phóng tiến trình gọi JoinWait().
    joinsem->V();
}

// Tiến trình con kết thúc
void PCB::ExitWait()
{ 
	// Gọi exitsem-->V() để tiến trình chuyển sang trạng thái block và ngừng lại, 
	// chờ ExitRelease để thực hiện tiếp.
    exitsem->P();
}

void PCB::IncNumWait()
{
	multex->P();
	numwait++;
	multex->V();
}

void PCB::DecNumWait()
{
	multex->P();
	if(numwait > 0)
		numwait--;
	multex->V();
}

int PCB::GetExitCode() {
	return this->exitcode;
}

void PCB::SetExitCode(int ec) {
	this->exitcode = ec;
}

void PCB::SetFileName(char* fn) {
	strcpy(filename, fn);
}

char* PCB::GetFileName() {
	return this->filename;
}