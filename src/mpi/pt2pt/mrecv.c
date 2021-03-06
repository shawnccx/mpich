/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2011 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "mpiimpl.h"

/* -- Begin Profiling Symbol Block for routine MPI_Mrecv */
#if defined(HAVE_PRAGMA_WEAK)
#pragma weak MPI_Mrecv = PMPI_Mrecv
#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#pragma _HP_SECONDARY_DEF PMPI_Mrecv  MPI_Mrecv
#elif defined(HAVE_PRAGMA_CRI_DUP)
#pragma _CRI duplicate MPI_Mrecv as PMPI_Mrecv
#elif defined(HAVE_WEAK_ATTRIBUTE)
int MPI_Mrecv(void *buf, int count, MPI_Datatype datatype, MPI_Message *message,
              MPI_Status *status) __attribute__((weak,alias("PMPI_Mrecv")));
#endif
/* -- End Profiling Symbol Block */

/* Define MPICH_MPI_FROM_PMPI if weak symbols are not supported to build
   the MPI routines */
#ifndef MPICH_MPI_FROM_PMPI
#undef MPI_Mrecv
#define MPI_Mrecv PMPI_Mrecv

/* any non-MPI functions go here, especially non-static ones */

#endif /* MPICH_MPI_FROM_PMPI */

#undef FUNCNAME
#define FUNCNAME MPI_Mrecv
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
/*@
MPI_Mrecv - Blocking receive of message matched by MPI_Mprobe or MPI_Improbe.

Input/Output Parameters:
. message - message (handle)

Input Parameters:
+ count - number of elements in the receive buffer (non-negative integer)
- datatype - datatype of each receive buffer element (handle)

Output Parameters:
+ buf - initial address of the receive buffer (choice)
- status - status object (status)

.N ThreadSafe

.N Fortran

.N Errors
@*/
int MPI_Mrecv(void *buf, int count, MPI_Datatype datatype, MPI_Message *message, MPI_Status *status)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_Request *msgp = NULL;
    MPIR_FUNC_TERSE_STATE_DECL(MPID_STATE_MPI_MRECV);

    MPID_THREAD_CS_ENTER(GLOBAL, MPIR_THREAD_GLOBAL_ALLFUNC_MUTEX);
    MPIR_FUNC_TERSE_ENTER(MPID_STATE_MPI_MRECV);

    /* Validate parameters, especially handles needing to be converted */
#   ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS
        {
            MPIR_ERRTEST_DATATYPE(datatype, "datatype", mpi_errno);

            /* TODO more checks may be appropriate */
        }
        MPID_END_ERROR_CHECKS
    }
#   endif /* HAVE_ERROR_CHECKING */

    /* Convert MPI object handles to object pointers */
    MPIR_Request_get_ptr(*message, msgp);

    /* Validate parameters and objects (post conversion) */
#   ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS
        {
            if (HANDLE_GET_KIND(datatype) != HANDLE_KIND_BUILTIN) {
                MPIR_Datatype *datatype_ptr = NULL;
                MPID_Datatype_get_ptr(datatype, datatype_ptr);
                MPIR_Datatype_valid_ptr(datatype_ptr, mpi_errno);
                if (mpi_errno != MPI_SUCCESS) goto fn_fail;
                MPID_Datatype_committed_ptr(datatype_ptr, mpi_errno);
                if (mpi_errno != MPI_SUCCESS) goto fn_fail;
            }

            /* MPI_MESSAGE_NO_PROC should yield a "proc null" status */
            if (*message != MPI_MESSAGE_NO_PROC) {
                MPIR_Request_valid_ptr(msgp, mpi_errno);
                if (mpi_errno) MPIR_ERR_POP(mpi_errno);
                MPIR_ERR_CHKANDJUMP((msgp->kind != MPIR_REQUEST_KIND__MPROBE),
                                    mpi_errno, MPI_ERR_ARG, "**reqnotmsg");
            }

            /* TODO more checks may be appropriate (counts, in_place, buffer aliasing, etc) */
        }
        MPID_END_ERROR_CHECKS
    }
#   endif /* HAVE_ERROR_CHECKING */

    /* ... body of routine ...  */

    mpi_errno = MPID_Mrecv(buf, count, datatype, msgp, status);
    if (mpi_errno) MPIR_ERR_POP(mpi_errno);

    *message = MPI_MESSAGE_NULL;

    /* ... end of body of routine ... */

fn_exit:
    MPIR_FUNC_TERSE_EXIT(MPID_STATE_MPI_MRECV);
    MPID_THREAD_CS_EXIT(GLOBAL, MPIR_THREAD_GLOBAL_ALLFUNC_MUTEX);
    return mpi_errno;

fn_fail:
    /* --BEGIN ERROR HANDLING-- */
#   ifdef HAVE_ERROR_CHECKING
    {
        mpi_errno = MPIR_Err_create_code(
            mpi_errno, MPIR_ERR_RECOVERABLE, FCNAME, __LINE__, MPI_ERR_OTHER,
            "**mpi_mrecv", "**mpi_mrecv %p %d %D %p %p", buf, count, datatype, message, status);
    }
#   endif
    mpi_errno = MPIR_Err_return_comm(NULL, FCNAME, mpi_errno);
    goto fn_exit;
    /* --END ERROR HANDLING-- */
}
