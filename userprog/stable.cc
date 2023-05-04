#include "stable.h"

// Constructor
STable::STable()
{
	this->bm = new BitMap(MAX_SEMAPHORE);
	for (int i = 0; i < MAX_SEMAPHORE; i++)
		this->semTab[i] = NULL;
}

// Destructor
STable::~STable()
{
	if (this->bm)
		delete this->bm;
	for (int i = 0; i < MAX_SEMAPHORE; i++)
		if (this->semTab[i])
			delete this->semTab[i];
}

// Kiểm tra Semaphore “name” chưa tồn tại thì tạo Semaphore mới. Ngược lại, báo lỗi.
int STable::Create(char *name, int init)
{
	// Kiểm tra tồn tại
	for (int i = 0; i < MAX_SEMAPHORE; i++)
		if (bm->Test(i))
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				DEBUG('a', "\nSemaphore %s has already existed!", name);
				printf("\nSemaphore %s has already existed!", name);
				return -1;
			}

	// Tìm slot trống trong bảng semTab
	int index = this->FindFreeSlot();

	// Nếu không có slot trống
	if (index == -1)
	{
		DEBUG('a', "\nNo free slot.");
		printf("\nNo free slot.");
		return -1;
	}

	// Tạo semaphore mới
	this->semTab[index] = new Sem(name, init);
	return 0;
}

int STable::Wait(char *name)
{
	// Kiểm tra tồn tại semaphore
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if (bm->Test(i))
		{
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				// Nếu tồn tại thì cho semaphore đợi
				semTab[i]->wait();
				return 0;
			}
		}
	}
	DEBUG('a', "\nSemaphore %s not exist!", name);
	printf("\nSemaphore %s not exist!", name);
	return -1;
}

int STable::Signal(char *name)
{
	// Kiểm tra tồn tại semaphore
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if (bm->Test(i))
		{
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				// Nếu tồn tại thì đánh thức semaphore
				semTab[i]->signal();
				return 0;
			}
		}
	}
	DEBUG('a', "\nSemaphore %s not exist!", name);
	printf("\nSemaphore %s not exist!", name);
	return -1;
}

// Tìm slot trống.
int STable::FindFreeSlot()
{
	return bm->Find();
}