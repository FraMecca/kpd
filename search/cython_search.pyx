cdef extern from "C_search.h":
    char **search_c_main (char *key, int *size, char *DBlocation, char *filterSt)
    void destroy_results (char **results, int size)
    void print_results (char **results, int size)


def search (key, DBlocation, filterSt):
    '''search library module.'''
    cdef int size = 0, i = 0
    cdef char** results

    if filterSt:
        results = search_c_main (key, &size, DBlocation, filterSt)
    else:
        results = search_c_main (key, &size, DBlocation, NULL)
    #print_results (results, size)
    retList = [st.decode ('utf-8') for st in results[:size]]
    destroy_results (results, size)
    return retList
