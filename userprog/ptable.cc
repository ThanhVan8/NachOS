#include "ptable.h"
#include "system.h"
#include "openfile.h"

#define For(i,x,y) for (int i = (x); i < y; ++i)
//constructor
PTable::PTable(int size)
{

    if (size < 0)
        return;

    psize = size;
    bm = new BitMap(size);
    bmsem = new Semaphore("bmsem",1);

    For(i,0,MAX_PROCESS){
		pcb[i] = 0;
    }

	bm->Mark(0);

	pcb[0] = new PCB(0);
	pcb[0]->SetFileName("./test/scheduler");
	pcb[0]->parentID = -1;
}

//destructor
PTable::~PTable()
{
    if( bm != 0 )
	delete bm;
    
    For(i,0,psize){
		if(pcb[i] != 0)
			delete pcb[i];
    }
		
	if( bmsem != 0)
		delete bmsem;
}

int PTable::JoinUpdate(int id)
{
	// kiểm tra tính hợp lệ của processID id
    if (id < 0 || id > MAX_PROCESS || !IsExist(id)) {
        printf("\nInvalid process ID!");
        DEBUG('a', "\nInvalid process ID!");
        return -1;
    }
    // kiểm tra tiến trình gọi Join có phải là cha 
	// của tiến trình có processID là id hay không
    if (currentThread->processID != pcb[id]->parentID) {
        printf("\nCurrent thread is not the parent of process %d", id);
        DEBUG('a', "\nCurrent thread is not the parent of process %d", id);
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

//tìm vị trí trống dùng hàm find của bitmap
int PTable::GetFreeSlot()
{
	return bm->Find();
}

// Kiểm tra process id
bool PTable::IsExist(int pid)
{
	return bm->Test(pid);
}

//remove process khi nó hoàn thành
void PTable::Remove(int pid)
{
	bm->Clear(pid);
	if(pcb[pid] != 0)
		delete pcb[pid];
}

//lấy tên tiến trình
char* PTable::GetFileName(int id)
{
	return (pcb[id]->GetFileName());
}

//xử lí ExecUpdate cho SC_Exec
int PTable::ExecUpdate(char* name)
{
    //Gọi bmsem->P(); để giúp tránh tình trạng nạp 2 tiến trình cùng 1 lúc.
	bmsem->P();
	
	// Kiểm tra tính hợp lệ của chương trình “name” và sự tồn tại của chương trình “name” bằng cách gọi phương thức Open của lớp fileSystem.
	if(name == NULL)
	{
		printf("\nPTable::Exec : Can't not execute name is NULL.\n");
		bmsem->V();
		return -1;
	}
	// So sánh tên chương trình và tên của currentThread để chắc chắn rằng chương trình này không gọi thực thi chính nó.
	if(strcmp(name,"./test/scheduler") == 0 || strcmp(name,currentThread->getName()) == 0 )
	{
		printf("\nPTable::Exec : Can't not execute itself.\n");		
		bmsem->V();
		return -1;
	}

	// Tìm vị trí trống trong bảng Ptable.
	int index = this->GetFreeSlot();

    // kiểm tra nếu không còn chỗ trống
	if(index < 0)
	{
		printf("\nPTable::Exec :There is no free slot.\n");
		bmsem->V();
		return -1;
	}

	//Khi tồn tại vị trí trống, tạo một PCB mới với processID chính là index của vị trí này
	pcb[index] = new PCB(index);
	pcb[index]->SetFileName(name); // gắn tên tiến trình

	// parrentID là processID của currentThread
    pcb[index]->parentID = currentThread->processID;


	// Gọi thực thi phương thức Exec của lớp PCB.
	int pid = pcb[index]->Exec(name,index);

	// Gọi bmsem->V()
	bmsem->V();
	// Trả về kết quả thực thi của PCB->Exec.
	return pid;
}