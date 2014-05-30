#include <stdio.h>
#include <stdlib.h>
#include "my402list.h"
//#include "warmup1.h"

//My402List * ListPtr;

int My402ListInit(My402List* list){
	list->num_members = 0;
	My402ListElem *	AnchorPtr;// = (My402ListElem*)malloc(sizeof(My402ListElem));
	AnchorPtr = &list->anchor;
	AnchorPtr->obj=NULL;
	AnchorPtr->next=NULL;
	AnchorPtr->prev=NULL;
	//printf("INIT: anchor addr:%d\n",AnchorPtr);
	if(list->num_members!=0)return 0;
	if(AnchorPtr!=&list->anchor)return 0;
	return 1;
}




int My402ListLength(My402List* list){
	return 	list->num_members;
}

int My402ListEmpty(My402List* list){
	if(list->num_members==0)return 1;
	else return 0;
}

My402ListElem* My402ListFirst(My402List* list){
	if(My402ListEmpty(list)==1){return NULL;}
	return (&list->anchor)->next;
}

My402ListElem* My402ListLast(My402List* list){
	if(My402ListEmpty(list)==1){return NULL;}
	return (&list->anchor)->prev;
}


int My402ListAppend(My402List* list, void* obj){
	My402ListElem* LastPtr;
	LastPtr = My402ListLast(list);

//*******************************************************************************************
//*******************************************************************************************
//
//
	My402ListElem * NewElemPtr = (My402ListElem*)malloc(sizeof(My402ListElem));
//
//
//*******************************************************************************************
//*******************************************************************************************

	if(LastPtr==NULL){
		NewElemPtr->obj = obj;
		NewElemPtr->prev = &list->anchor;
		NewElemPtr->next = &list->anchor;

		(&list->anchor)->prev = NewElemPtr;
		(&list->anchor)->next = NewElemPtr;

		if( (NewElemPtr->next != &list->anchor)||(NewElemPtr->prev != &list->anchor)||(NewElemPtr->obj != obj)||((&list->anchor)->prev!=NewElemPtr)||((&list->anchor)->next!=NewElemPtr) )return 0;

		list->num_members++;
//printf("LAST=NULL :#%d\n",list->num_members);
	}
	else{
		NewElemPtr->obj = obj;
		NewElemPtr->prev = LastPtr;
		NewElemPtr->next = &list->anchor;

		LastPtr->next = NewElemPtr;
		(&list->anchor)->prev = NewElemPtr;

		if( (NewElemPtr->next != &list->anchor)||(NewElemPtr->prev != LastPtr)||(NewElemPtr->obj != obj)||((&list->anchor)->prev!=NewElemPtr)||(LastPtr->next!=NewElemPtr) )return 0;

		list->num_members++;
//printf("LAST:#%d\n",list->num_members);
	}

	return 1;
}





int My402ListPrepend(My402List* list, void* obj){
	My402ListElem* FirstPtr;
	FirstPtr = My402ListFirst(list);

	My402ListElem * NewElemPtr = (My402ListElem*)malloc(sizeof(My402ListElem));

	if(FirstPtr==NULL){
		NewElemPtr->obj = obj;
		NewElemPtr->prev = &list->anchor;
		NewElemPtr->next = &list->anchor;

		(&list->anchor)->prev = NewElemPtr;
		(&list->anchor)->next = NewElemPtr;

		if( (NewElemPtr->next != &list->anchor)||(NewElemPtr->prev != &list->anchor)||(NewElemPtr->obj != obj)||((&list->anchor)->prev!=NewElemPtr)||((&list->anchor)->next!=NewElemPtr) )return 0;

		list->num_members++;	
	}
	else{
		NewElemPtr->obj = obj;
		NewElemPtr->prev = &list->anchor;
		NewElemPtr->next = FirstPtr;

		FirstPtr->prev = NewElemPtr;
		(&list->anchor)->next = NewElemPtr;

		if( (NewElemPtr->next != FirstPtr)||(NewElemPtr->prev != &list->anchor)||(NewElemPtr->obj != obj)||((&list->anchor)->next!=NewElemPtr)||(FirstPtr->prev!=NewElemPtr) )return 0;

		list->num_members++;
	}

	
	return 1;
}

int My402ListInsertBefore(My402List* list, void* obj, My402ListElem* elem){

	if(elem==NULL){
		My402ListPrepend(list,obj);
		return 1;
	}
	
	My402ListElem * PrevPtr;
	PrevPtr = My402ListPrev(list,elem);


//************************************************************************************************
//************************************************************************************************
//
//
	My402ListElem * NewElemPtr = (My402ListElem*)malloc(sizeof(My402ListElem));
//
//
//************************************************************************************************
//************************************************************************************************

	if(PrevPtr==NULL){
		NewElemPtr->obj = obj;
		NewElemPtr->next = elem;
		NewElemPtr->prev = &list->anchor;

		(&list->anchor)->next = NewElemPtr;
		elem->prev = NewElemPtr;

		if((NewElemPtr->next != elem)||(NewElemPtr->prev != &list->anchor)||(NewElemPtr->obj != obj)||((&list->anchor)->next != NewElemPtr)||(elem->prev!=NewElemPtr))return 0;

		list->num_members++;
	}
	else{
		NewElemPtr->obj = obj;
		NewElemPtr->next = elem;
		NewElemPtr->prev = PrevPtr;

		PrevPtr->next = NewElemPtr;
		elem->prev = NewElemPtr;

		if((NewElemPtr->next != elem)||(NewElemPtr->prev != PrevPtr)||(NewElemPtr->obj != obj)||(PrevPtr->next != NewElemPtr)||(elem->prev!=NewElemPtr))return 0;

		list->num_members++;

	}
	
	return 1;	
}


int My402ListInsertAfter(My402List* list, void* obj, My402ListElem* elem){

	if(elem==NULL){
		My402ListAppend(list,obj);
		return 1;
	}

	My402ListElem * NextPtr;
	NextPtr = My402ListNext(list,elem);

	My402ListElem * NewElemPtr = (My402ListElem*)malloc(sizeof(My402ListElem));

	if(NextPtr==NULL){
		NewElemPtr->obj = obj;
		NewElemPtr->next = &list->anchor;
		NewElemPtr->prev = elem;

		(&list->anchor)->prev = NewElemPtr;
		elem->next = NewElemPtr;

		if((NewElemPtr->next != &list->anchor)||(NewElemPtr->prev != elem)||(NewElemPtr->obj != obj)||(elem->next != NewElemPtr)||((&list->anchor)->prev!=NewElemPtr))return 0;

		list->num_members++;
	}
	else{
		NewElemPtr->obj = obj;
		NewElemPtr->next = NextPtr;
		NewElemPtr->prev = elem;

		NextPtr->prev = NewElemPtr;
		elem->next = NewElemPtr;

		if((NewElemPtr->next != NextPtr)||(NewElemPtr->prev != elem)||(NewElemPtr->obj != obj)||(elem->next != NewElemPtr)||(NextPtr->prev!=NewElemPtr))return 0;

		list->num_members++;

	}


	return 1;	
}





My402ListElem* My402ListNext(My402List *list, My402ListElem* ListElem){
	if(ListElem->next==&list->anchor){
		//printf("next=last elem current:%d, next:%d\n",*(ListElem->data),*(ListElem->next->data));
		return NULL;
	}
	else{
		//printf("next=not last elem current:%d, next:%d\n",*(ListElem->data),*(ListElem->next->data));
		return ListElem->next;
	}
}


My402ListElem* My402ListPrev(My402List *list, My402ListElem* ListElem){
	if(ListElem->prev==&list->anchor){
		//printf("prev=last\n");
		return NULL;
	}
	else{ 
		//printf("next=not last\n"); 
		return ListElem->prev;
	}
}



My402ListElem* My402ListFind(My402List* list, void* obj){
//printf("to find:%ld\n",*(long*)obj);
	if(My402ListEmpty(list)==1){return NULL;}
	My402ListElem * CurrentPtr = My402ListFirst(list);
//	MyStruct *myStr = (MyStruct*)malloc(sizeof(MyStruct));
	int flag=0;
	while( (CurrentPtr != NULL)){
//	myStr = (MyStruct*)CurrentPtr->obj;
//printf("	matching with:%ld\n",myStr->longTime);
//		if(myStr->longTime==*(long*)obj){
		if(CurrentPtr->obj==obj){
			//printf("\nmatch found\n");
			flag=1;
			break;
		}
		CurrentPtr = My402ListNext(list,CurrentPtr);
	}
	if(flag==1){return CurrentPtr;}
	else{return NULL;}
}


//ASK: prev & next is it assumed that the elem passed is NOT anchor?
//ASK: unlinkAll just remove links of anchor? what about deleting elements
void My402ListUnlink(My402List* list,My402ListElem* elem){
	if(elem==NULL){
		//printf("No Element specified to unlink\n");
		return;
	}

	My402ListElem * PrevPtr = My402ListPrev(list,elem);
	My402ListElem * NextPtr = My402ListNext(list,elem);
	

	if(My402ListEmpty(list)==1){
		//printf("List Empty\n");
		return;
	}

	//the one and only elem to unlink
	if( (PrevPtr == NULL)&&(NextPtr==NULL) ){

		(&list->anchor)->next = &list->anchor;
		(&list->anchor)->prev = &list->anchor;

		list->num_members = 0;

		//printf("List is NOW Empty\n");
		free(elem);
		return;
	}

	//first elem to unlink
	if(PrevPtr == NULL){
		(&list->anchor)->next = NextPtr;
		NextPtr->prev = &list->anchor;
	
		list->num_members--;
		free(elem);
		//printf("FIRST:the no. of members: %d\n",list->num_members);

		return;
	}

	//last elem to unlink
	if(NextPtr == NULL){
		(&list->anchor)->prev = PrevPtr;
		PrevPtr->next = &list->anchor;
	
		list->num_members--;

		//printf("LAST:the no. of members: %d\n",list->num_members);
		free(elem);
		return;
	}
	PrevPtr->next = NextPtr;
	NextPtr->prev = PrevPtr;

	list->num_members--;

	free(elem);

	//printf("#members: %d\n",list->num_members);
	return;
}


void My402ListUnlinkAll	(My402List* list){

	if(My402ListEmpty(list) == 1){
		//printf("List is already Empty\n");
		return;
	}


//free(My402ListFirst(list));

	My402ListElem *prev,*current;
	prev = My402ListFirst(list);
	current = prev;

	while(current!=NULL){
		current = My402ListNext(list,prev);
		free(prev);		
		prev = current;
	}
	


	list->num_members = 0;
	(&list->anchor)->next = &list->anchor;
	(&list->anchor)->prev = &list->anchor;

	return;
}


