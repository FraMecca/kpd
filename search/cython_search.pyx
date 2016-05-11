cdef extern from "C_search.h":
    char **search_c_main (char *key, int *size, char *DBlocation, char *filterSt, char *revFilterSt)
    void destroy_results (char **results, int size)
    void print_results (char **results, int size)


def search (key, DBlocation, filterSt, revFilterSt):
    '''search library module.'''
    cdef int size = 0, i = 0
    cdef char** results
    cdef char *fst, *frt

    if not filterSt:
        fst = NULL
    else:
        fst = filterSt
    if not revFilterSt:
        frt = NULL
    else:
        frt = revFilterSt
    results = search_c_main (key, &size, DBlocation, fst, frt)
    #print_results (results, size)
    retList = [st.decode ('utf-8') for st in results[:size]]
    destroy_results (results, size)
    return retList
