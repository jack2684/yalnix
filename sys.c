#include "kernelLib.h"

//These are destroy syscalls
int Y_Reclaim(int id){
	//FIND the lock using id
	//DESTROY the lock

	//FIND the cvar using id
	//DESTROY the cvar

	//FIND the pipe using id
	//DESTROY the pipe
}

//Here are some extra syscalls below that could help implement some extra functions
//We will complete them if possible

//These are 3 custom syscalls
int Y_Custom0(){
	
}

int Y_Custom1(){
	
}

int Y_Custom2(){
	
}

int Y_Nop(int, int, int, int){

}

int Y_Boot(){

}

int Y_Register(unsigned int **){
	
}

int Y_Send(void* , int){
	
}

int Y_Receive(void *){

}

int Y_ReceiveSpecific(void *, int ){

}

int Y_Reply (void *, int){

}

int Y_Forward (void *, int, int){

}

int Y_CopyFrom (int, void *, void *, int){

}

int Y_CopyTo (int, void *, void *, int){

}

int Y_ReadSector (int, void *){

}

int Y_WriteSector (int, void *){

}
