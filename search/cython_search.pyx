cdef extern from "C_search.h":
    void search_c_main (char *key)
    
def search (key):
    '''search library module.'''
    search_c_main (key)
