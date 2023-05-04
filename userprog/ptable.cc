#include "ptable.h"
#include "system.h"
#include "openfile.h"

//#define For(i,x,y) for (int i = (x); i < y; ++i)
//constructor
PTable::PTable(int size)
{
    if (size < 0 || size > MAX_PROCESS)
        size = MAX_PROCESS;
    psize = size;
	for (int i = 0; i < MAX_PROCESS; i++)
		pcb[i] = NULL;

    bm = new BitMap(size);
    bmsem = new Semaphore("bmsem", 1);

    // For(i,0,MAX_PROCESS){
	// 	pcb[i] = 0;
    // }

	bm->Mark(0);

	pcb[0] = new PCB(0);
	//pcb[0]->parentID = -1;
	pcb[0]->SetFileName("./test/scheduler");
}

//destructor
PTable::~PTable()
{
	if (bm)
		delete bm;
	if (bmsem)
		delete bmsem;
	for (int i = 0; i < psize; i++)
		if (pcb[i])
			delete pcb[i];
}

// xử lí ExecUpdate cho SC_Exec
int PTable::ExecUpdate(char *name)
{
	// Gọi bmsem->P(); để giúp tránh tình trạng nạp 2 tiến trình cùng 1 lúc.
	bmsem->P();

	// Kiểm tra tính hợp lệ của chương trình “name”.
	if (name == NULL)
	{
		DEBUG('a', "\nName is NULL!");
		printf("\nName is NULL!");
		bmsem->V();
		return -1;
	}
	// Kiểm tra sự tồn tại của chương trình “name” bằng cách gọi phương thức Open của lớp fileSystem
	OpenFile *executable = fileSystem->Open(name);
	if (executable == NULL)
	{
		DEBUG('a', "\nNot exist thread %s", name);
		printf("\nNot exist thread %s", name);
		bmsem->V();
		return -1;
	}

	// So sánh tên chương trình và tên của currentThread để chắc chắn rằng chương trình này không gọi thực thi chính nó.
	// if(strcmp(name,"./test/scheduler") == 0 || strcmp(name,currentThread->getName()) == 0 )
	if (strcmp(name, currentThread->getName()) == 0)
	{
		DEBUG('a', "\nCannot execute itself.");
		printf("\nCannot execute itself.");
		bmsem->V();
		return -1;
	}

	// Tìm slot trống trong bảng Ptable.
	int index = this->GetFreeSlot();

	// Nếu không có slot trống
	if (index == -1)
	{
		DEBUG('a', "\nNo free slot.");
		printf("\nNo free slot.");
		bmsem->V();
		return -1;
	}

	// Nếu có slot trống thì khởi tạo một PCB mới với processID chính là index của slot này, parrentID là
	// processID của currentThread.
	pcb[index] = new PCB(index);
	pcb[index]->parentID = currentThread->processID;
	pcb[index]->SetFileName(name);

	// Gọi thực thi phương thức Exec của lớp PCB.
	int pid = pcb[index]->Exec(name, index);

	// Gọi bmsem->V()
	bmsem->V();

	// Trả về kết quả thực thi của PCB->Exec.
	return pid;
}

int PTable::JoinUpdate(int id)
{
	// kiểm tra tính hợp lệ của processID id
    if (id < 0 || id > MAX_PROCESS || !IsExist(id)) {
        DEBUG('a', "\nInvalid process ID!");
        printf("\nInvalid process ID!");
        return -1;
    }
    // kiểm tra tiến trình gọi Join có phải là cha 
	// của tiến trình có processID là id hay không
    if (currentThread->processID != pcb[id]->parentID) {
        DEBUG('a', "\nCurrent thread is not the parent of process %d", id);
        printf("\nCurrent thread is not the parent of process %d", id);
        return -1;
    }
    // Tăng numwait và gọi JoinWait() để chờ tiến trình con thực hiện.
    pcb[currentThread->processID]->IncNumWait();
    pcb[id]->JoinWait();
    // Xử lý exitcode.
    int exitcode = pcb[id]->GetExitCode();
    // ExitRelease() để cho phép tiến trình con thoát.
	pcb[id]->ExitRelease();
	return exitcode;
}

int PTable::ExitUpdate(int ec)
{              
    // Nếu tiến trình gọi là main process thì gọi Halt().
	int pid = currentThread->processID;
	if (pid == 0) {
		currentThread->FreeSpace();
		interrupt->Halt();
		return 0;
	}
	// Ngược lại gọi SetExitCode để đặt exitcode cho tiến trình gọi.
	if (!IsExist(pid)) {
		printf("\nInvalid process ID!");
        DEBUG('a', "\nInvalid process ID!");
        return -1;
	}
	pcb[pid]->SetExitCode(ec);
	// Gọi JoinRelease để giải phóng tiến trình cha đang đợi nó(nếu có) và ExitWait() để xin tiến trình cha 
	// cho phép thoát.
	pcb[pcb[pid]->parentID]->DecNumWait();
	pcb[pid]->JoinRelease();
	pcb[pid]->ExitWait();
	this->Remove(pid);
	
	// giải phóng tiểu trình hiện tại
	currentThread->FreeSpace();
	currentThread->Finish();
	return ec;
}

// tìm free slot để lưu thông tin cho tiến trình mới
int PTable::GetFreeSlot()
{
	return bm->Find();
}

// kiểm tra tồn tại processID này không?
bool PTable::IsExist(int pid)
{
	return bm->Test(pid);
}

// khi tiến trình kết thúc, delete processID ra khỏi mảng quản lý nó
void PTable::Remove(int pid)
{
	bm->Clear(pid);
	if (pcb[pid])
		delete pcb[pid];
}

// Trả về tên của tiến trình
char *PTable::GetFileName(int id)
{
	return pcb[id]->GetFileName();
}