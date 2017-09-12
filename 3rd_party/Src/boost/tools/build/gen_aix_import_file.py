#!/usr/bin/env python

# Valid arguments
setValueArgs = {"-ld" : "LD",
                "-ldflags" : "LDFLAGS",
                "-optldflags" : "OPTLDFLAGS",
                "-impfilename" : "IMPFILENAME",
                "-expflag" : "EXPFLAG",
                "-maxlinelength" : "maxlinelength",
                "-verbose" : "chatty",
                "-directory" : "CHDIR" }
validArgs = setValueArgs.keys()

# Little function to generate a usage statement.
def printUsage():
    print "Usage: generateImportFile <target lib base name>"

# Helper to execute the given command string in the host OS.
def execCommand(command, chatty):
    if chatty:
        print command
    import os
    os.system(command)

# Default values for the important vars
LD = "xlC_r"
LDFLAGS = "-G -qmkshrobj"
OPTLDFLAGS = ""
EXPFLAG = "-qexpfile"
maxlinelength = 4095
OBJS = ""
chatty = None

import sys
nargs = len(sys.argv)
if nargs < 2:
    printUsage()
    exit(1)

# Get the name of the lib target.
LIBTARGET = sys.argv[1]
if LIBTARGET[-3:] != ".so":
    LIBTARGET += ".so"

# The default name for the export and import files we'll be generating.
EXPFILENAME = LIBTARGET[:-3] + ".exp"
IMPFILENAME = LIBTARGET[:-3] + ".imp"

CHDIR = "."

# Parse optional arguments
for iarg in xrange(2, nargs):
    arg = sys.argv[iarg]

    # Is this a -thingy=thingyValue argument?
    if arg[0] == "-":
        assert arg.count("=") == 1
        argKey, argValue = arg.split("=")
        if argKey not in validArgs:
            printUsage()
            exit(1)

        exec("%s = '%s'" % (setValueArgs[argKey], argValue))
        print "Setting " + setValueArgs[argKey] + " to " + argValue
        
    # Is this an object file/lib that needs to be linked in?
    elif arg[-2:] == ".o" or arg[-2:] == ".a" or arg[-3:] == ".so":
        OBJS += arg + " "

    else:
        printUsage()
        exit()

import os
os.chdir(CHDIR)

# Generate the export file by creating the shared obj library, which we then
# promptly delete.
command = LD + " " + LDFLAGS + " " + OPTLDFLAGS + " " + \
          EXPFLAG + "=" + EXPFILENAME + " " + \
          OBJS + " -o " + LIBTARGET 
execCommand(command, chatty)
execCommand("rm -f " + LIBTARGET, chatty)

# Create the import file, and put the required tag as the first line indicating
# the name of the archive which is exporting these symbols.
impfile = open(IMPFILENAME, 'w')
impfile.write("#!" + LIBTARGET + "\n")

# Attach the list of symbols being exported to the import file.
expfile = open(EXPFILENAME, 'r')
symbol = expfile.readline()
while symbol:
    if len(symbol) <= maxlinelength:
        if symbol[-1] != '\n':
            symbol += '\n'
        impfile.write(symbol)
    else:
        print 'skipping', str(len(symbol)) + '-character symbol:',str(symbol)
    symbol = expfile.readline()
impfile.close()
expfile.close()

#command = "cat " + EXPFILENAME + " >> " + IMPFILENAME
#execCommand(command, chatty)

# Remove the export file.
execCommand("rm -f " + EXPFILENAME, chatty)
