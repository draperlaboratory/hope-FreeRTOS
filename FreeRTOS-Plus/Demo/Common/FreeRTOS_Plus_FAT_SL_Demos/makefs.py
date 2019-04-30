#!/usr/bin/env python

# File: makefs.py
# Copyright (c) 2018, DornerWorks, Ltd.
#
# Description:
#   This file converts HTML and PNG files to C arrays for the FreeRTOS FAT filesystem.

# Copyright DornerWorks 2018
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1.  Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DORNERWORKS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import sys, os, glob

WORKING_DIR = os.path.dirname(os.path.abspath(__file__))
FILES_SRC_DIR = os.path.join(WORKING_DIR, 'www/')
FILES_DST_DIR = os.path.join(WORKING_DIR, 'CreateExampleFiles/generated/')
SRC_FILES = glob.glob(FILES_SRC_DIR + '*.html')
TEXT_FILE_TYPES = ['html']
NEW_FS_HEADER_NAME = 'generated_files'
NEW_FS_HEADER = FILES_DST_DIR + NEW_FS_HEADER_NAME + '.h'

def create_root_header():
    includes = ""
    for src_file in SRC_FILES:
        header = os.path.split(src_file)[1]
        new_name = header.replace('.', '_').replace('-', '_')
        header_file = FILES_DST_DIR + new_name + '.h'
        if os.path.isfile(header_file):
            includes += "#include \"%s.h\"\n" % new_name

    with open(NEW_FS_HEADER, 'w+') as fs_header:
        fs_header.write("/* WARNING: Automatically Generated File */\n\n")
        fs_header.write("#ifndef %s\n" % NEW_FS_HEADER_NAME.upper())
        fs_header.write("#define %s\n\n" % NEW_FS_HEADER_NAME.upper())
        fs_header.write("%s" % includes)
        fs_header.write("\n#endif /* %s */\n" % NEW_FS_HEADER_NAME.upper())

def clean_directory():
    if os.path.isfile(NEW_FS_HEADER):
        os.remove(NEW_FS_HEADER)
    for src_file in SRC_FILES:
        header = os.path.split(src_file)[0]
        header_file = FILES_DST_DIR + header.replace('.', '_').replace('-', '_') + '.h'
        if os.path.isfile(header_file):
            os.remove(header_file)

#
# Convert all the HTML and PNG files in the 'www/' directory to header files.
# something.html gets converted to an uint8_t data_something_html[] = ...
#
# To add different file extensions, add a line to the SRC_FILES with the proper extension.
def files_to_array():
    # TODO: Refactor this function to handle single file in directory w/o so many loops
    clean_directory()
    if not os.path.isdir(FILES_DST_DIR):
        os.makedirs(FILES_DST_DIR)
    for src_file in SRC_FILES:
        with open(src_file) as temp_file:
            file_hex = ''.join(temp_file.readlines()).encode('hex')
            header = os.path.split(src_file)[1]
            name = header.replace('.', '_').replace('-', '_')
            header_file = FILES_DST_DIR + name + '.h'
            with open(header_file, 'w+') as new_header:
                new_header.write("/* WARNING: Automatically Generated File */\n\n")
                new_header.write("#ifndef %s\n" % name.upper())
                new_header.write("#define %s\n\n" % name.upper())
                # Create an array. Right now, file is in hex stream (012ac45e1) which needs to be converted
                # to individual values (0x12, 0xac, 0x45, 0xe1), then format and close the array
                new_header.write("static const uint8_t %s[] = {\n" % name)
                hex_codes = ', '.join(['0x' + file_hex[i:i+2] for i in range(0, len(file_hex), 2)]) + ','
                if header.split('.')[1] in TEXT_FILE_TYPES:
                    hex_codes = hex_codes.replace('0x0a,', '0x0d, 0x0a,')
                hex_codes = '\n'.join([hex_codes[i:i+96].rstrip() for i in range(0, len(hex_codes), 96)])
                new_header.write("%s\n};\n\n" % str(hex_codes))
                new_header.write("#endif /* %s */\n" % name.upper())

if __name__ == "__main__":
    if ''.join(sys.argv[1:]) == "-c":
        clean_directory()
        sys.exit(0)
    files_to_array()
    create_root_header()
