import os.path
from Exporter import Exporter

#==============================================================================
# IncludeExporter
#==============================================================================
class IncludeExporter(Exporter):
    '''Writes an include declaration to the module. Useful to add extra code
    for use in the Wrappers.
    This class just reimplements the Parse method to do nothing: the 
    WriteInclude in Exporter already does the work for us.
    '''

    def __init__(self, info, parser_tail=None):
        Exporter.__init__(self, info, parser_tail)

    def Parse(self, parser):
        pass

