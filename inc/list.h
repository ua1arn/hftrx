/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include <stddef.h>
#include <stdint.h>


#define NTAPI
#define NTSYSAPI
#define IN_param
#define OUT_param

typedef unsigned long ULONG, * PULONG;
typedef signed long LONG, * PLONG;
typedef void VOID, * PVOID;

typedef signed long long LONGLONG;
typedef unsigned long long ULONGLONG;

#define RESTRICTED_POINTER /* */
#define IN_param /* */
#define OUT_param /* */

typedef unsigned short USHORT;

#if defined(MIDL_PASS)
typedef struct _LARGE_INTEGER {
#else // MIDL_PASS
typedef union _LARGE_INTEGER {
    struct {
        ULONG LowPart;
        LONG HighPart;
    } s;
    struct {
        ULONG LowPart;
        LONG HighPart;
    };
#endif //MIDL_PASS
    LONGLONG QuadPart;
} LARGE_INTEGER;

typedef LARGE_INTEGER *PLARGE_INTEGER;


#if defined(MIDL_PASS)
typedef struct _ULARGE_INTEGER {
#else // MIDL_PASS
typedef union _ULARGE_INTEGER {
    struct {
        ULONG LowPart;
        ULONG HighPart;
    } s;
    struct {
        ULONG LowPart;
        ULONG HighPart;
    };
#endif //MIDL_PASS
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

typedef ULARGE_INTEGER *PULARGE_INTEGER;
//
//  Doubly linked list structure.  Can be used as either a list head, or
//  as link words.
//

typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY, *RESTRICTED_POINTER PRLIST_ENTRY;

//
//  Singly linked list structure. Can be used as either a list head, or
//  as link words.
//

typedef struct _SINGLE_LIST_ENTRY {
    struct _SINGLE_LIST_ENTRY *Next;
} SINGLE_LIST_ENTRY, *PSINGLE_LIST_ENTRY;


typedef union _SLIST_HEADER {
    ULONGLONG Alignment;
    struct {
        SINGLE_LIST_ENTRY Next;
        USHORT Depth;
        USHORT Sequence;
    };
} SLIST_HEADER, *PSLIST_HEADER;


//
// Define interlocked sequenced listhead functions.
//
// A sequenced interlocked list is a singly linked list with a header that
// contains the current depth and a sequence number. Each time an entry is
// inserted or removed from the list the depth is updated and the sequence
// number is incremented. This enables MIPS, Alpha, and Pentium and later
// machines to insert and remove from the list without the use of spinlocks.
// The PowerPc, however, must use a spinlock to synchronize access to the
// list.
//
// N.B. A spinlock must be specified with SLIST operations. However, it may
//      not actually be used.
//

/*++

VOID
ExInitializeSListHead (
    IN_param PSLIST_HEADER SListHead
    )

Routine Description:

    This function initializes a sequenced singly linked listhead.

Arguments:

    SListHead - Supplies a pointer to a sequenced singly linked listhead.

Return Value:

    None.

--*/

#define ExInitializeSListHead(_listhead_) (_listhead_)->Alignment = 0

/*++

USHORT
ExQueryDepthSList (
    IN_param PSLIST_HEADERT SListHead
    )

Routine Description:

    This function queries the current number of entries contained in a
    sequenced single linked list.

Arguments:

    SListHead - Supplies a pointer to the sequenced listhead which is
        be queried.

Return Value:

    The current number of entries in the sequenced singly linked list is
    returned as the function value.

--*/

#define ExQueryDepthSList(_listhead_) (USHORT)(_listhead_)->Depth

//
// Determine if an argument is present by testing the value of the pointer
// to the argument value.
//

#define ARGUMENT_PRESENT(ArgumentPointer)    (\
    (char *)(ArgumentPointer) != (char *)(NULL) )

//
// Calculate the address of the base of the structure given its type, and an
// address of a field within the structure.
//

#define CONTAINING_RECORD(address, type, field) ( (type *)( \
                                                  (uint8_t *)(address) - \
                                                  (uintptr_t) offsetof(type, field) ))

/*
 *	functions.
 */

///////
//
VOID 
InitializeListHead(
	IN_param PLIST_ENTRY  ListHead
	); 

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

///////
//
VOID 
InsertHeadList( 
	IN_param PLIST_ENTRY ListHead, 
	IN_param PLIST_ENTRY Entry
	);

#define InsertHeadList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Flink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }


///////
//
VOID 
InsertTailList( 
	IN_param PLIST_ENTRY ListHead, 
	IN_param PLIST_ENTRY Entry
	);

#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

///////
//
int 
IsListEmpty(
	IN_param PLIST_ENTRY ListHead
	);

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

///////
//
PLIST_ENTRY 
RemoveHeadList( 
	IN_param PLIST_ENTRY ListHead 
	);

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}


///////
//
PLIST_ENTRY 
RemoveTailList( 
	IN_param PLIST_ENTRY ListHead 
	);

#define RemoveTailList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryList((ListHead)->Blink)}


///////
//
VOID 
RemoveEntryList( 
	IN_param PLIST_ENTRY Entry
	);

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }


//
//
PSINGLE_LIST_ENTRY
PopEntryList(
	PSINGLE_LIST_ENTRY ListHead
	);

#define PopEntryList(ListHead) \
    (ListHead)->Next;\
    {\
        PSINGLE_LIST_ENTRY FirstEntry;\
        FirstEntry = (ListHead)->Next;\
        if (FirstEntry != NULL) {     \
            (ListHead)->Next = FirstEntry->Next;\
        }                             \
    }


//
VOID
PushEntryList(
	PSINGLE_LIST_ENTRY ListHead,
	PSINGLE_LIST_ENTRY Entry
	);


#define PushEntryList(ListHead,Entry) \
    (Entry)->Next = (ListHead)->Next; \
    (ListHead)->Next = (Entry)




#endif /* LIST_H_INCLUDED */
