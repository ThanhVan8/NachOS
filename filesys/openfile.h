#ifndef OPENFILE_H
#define OPENFILE_H

#include "copyright.h"
#include "utility.h"

#define MaxFileLength 32

#ifdef FILESYS_STUB			// Temporarily implement calls to 
					// Nachos file system as calls to UNIX!
					// See definitions listed under #else
class OpenFile {
  public:
  	int type;	// 0: file doc va ghi
				// 1: file chi doc
		
	OpenFile(int f) { file = f; currentOffset = 0; type = 0;}
	OpenFile(int f, int t) { file = f; currentOffset = 0; type = t;}	// mo file voi type
    	~OpenFile() { Close(file); }			// close the file

    int ReadAt(char *into, int numBytes, int position) { 
    		Lseek(file, position, 0); 
		return ReadPartial(file, into, numBytes); 
		}	
    int WriteAt(char *from, int numBytes, int position) { 
    		Lseek(file, position, 0); 
		WriteFile(file, from, numBytes); 
		return numBytes;
		}	
    int Read(char *into, int numBytes) {
		int numRead = ReadAt(into, numBytes, currentOffset); 
		currentOffset += numRead;
		return numRead;
    		}
    int Write(char *from, int numBytes) {
		int numWritten = WriteAt(from, numBytes, currentOffset); 
		currentOffset += numWritten;
		return numWritten;
		}

    int Length() { Lseek(file, 0, 2); return Tell(file); }
	
  private:
    int file;
    int currentOffset;
};

#else // FILESYS
class FileHeader;

class OpenFile {
  public:
  	//Khai bao bien type
  	int type; 		// 0: file doc va ghi
					// 1: file chi doc

	
    OpenFile(int sector);		// Open a file whose header is located
					// at "sector" on the disk
    OpenFile(int sector, int type);	

    ~OpenFile();			// Close the file

    void Seek(int position); 		// Set the position from which to 
					// start reading/writing -- UNIX lseek
	
    int Read(char *into, int numBytes); // Read/write bytes from the file,
					// starting at the implicit position.
					// Return the # actually read/written,
					// and increment position in file.
    int Write(char *from, int numBytes);

    int ReadAt(char *into, int numBytes, int position);
    					// Read/write bytes from the file,
					// bypassing the implicit position.
    int WriteAt(char *from, int numBytes, int position);

    int Length(); 			// Return the number of bytes in the
					// file (this interface is simpler 
					// than the UNIX idiom -- lseek to 
					// end of file, tell, lseek back 

  private:
    FileHeader *hdr;			// Header for this file 
    int seekPosition;			// Current position within the file
};

#endif // FILESYS

#endif // OPENFILE_H
