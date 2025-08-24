/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include <stddef.h>

#if defined(_MSC_VER) && (_MSC_VER < 1600)
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
#else
#include <stdint.h>
#include <stdlib.h>
#endif


#define NTAPI
#define NTSYSAPI
#define IN_param
#define OUT_param

typedef unsigned long ULONG, * PULONG;
typedef long LONG, * PLONG;
typedef void VOID, * PVOID;

typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;

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

typedef struct _VLIST_ENTRY {
   volatile struct _VLIST_ENTRY *Flink;
   volatile struct _VLIST_ENTRY *Blink;
} VLIST_ENTRY, *RESTRICTED_POINTER PRVLIST_ENTRY;
typedef volatile VLIST_ENTRY * PVLIST_ENTRY;

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
                                                  (unsigned char *)(address) - \
                                                  (unsigned) offsetof(type, field) ))

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

#define InsertHeadVList(ListHead,Entry) {\
    PVLIST_ENTRY _EX_Flink;\
    PVLIST_ENTRY _EX_ListHead;\
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

#define InsertTailVList(ListHead,Entry) {\
    PVLIST_ENTRY _EX_Blink;\
    PVLIST_ENTRY _EX_ListHead;\
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

#define RemoveHeadVList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryVList((ListHead)->Flink)}


///////
//
PLIST_ENTRY 
RemoveTailList( 
	IN_param PLIST_ENTRY ListHead 
	);

#define RemoveTailList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryList((ListHead)->Blink)}

#define RemoveTailVList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryVList((ListHead)->Blink)}


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

#define RemoveEntryVList(Entry) {\
    PVLIST_ENTRY _EX_Blink;\
    PVLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

//
// Taken from https://github.com/vathpela/gnu-efi/blob/master/inc/efilink.h
//
//  VOID
//  SwapListEntries(
//      PLIST_ENTRY Entry1,
//      PLIST_ENTRY Entry2
//      );
//
// Put Entry2 before Entry1
//

#define SwapListEntries(Entry1,Entry2) {\
    LIST_ENTRY *Entry1Flink, *Entry1Blink;     \
    LIST_ENTRY *Entry2Flink, *Entry2Blink;     \
    Entry2Flink = (Entry2)->Flink;             \
    Entry2Blink = (Entry2)->Blink;             \
    Entry1Flink = (Entry1)->Flink;             \
    Entry1Blink = (Entry1)->Blink;             \
    Entry2Blink->Flink = Entry2Flink;       \
    Entry2Flink->Blink = Entry2Blink;        \
    (Entry2)->Flink = Entry1;               \
    (Entry2)->Blink = Entry1Blink;          \
    Entry1Blink->Flink = (Entry2);            \
    (Entry1)->Blink = (Entry2);             \
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
