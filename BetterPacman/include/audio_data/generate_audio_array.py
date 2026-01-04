#Create c array from raw data
"""

    @file: generate_audio_array.py
    @author: Zoey Halligan

    @description: 
        Creates the C array to be used by the DAC 
        for the audio of Pacman eating a pellet.

"""

import sys

def generate_c_array(input_filename, array_name="audio_data"):
    with open(input_filename, "rb") as f:
        data = f.read()

    header_filename = f"{array_name}.h"
    source_filename = f"{array_name}.c"

    # Generate the .h file
    with open(header_filename, "w") as h_file:
        h_file.write(f"#ifndef {array_name.upper()}_H\n")
        h_file.write(f"#define {array_name.upper()}_H\n\n")
        h_file.write(f"#include <stdint.h>\n\n")
        h_file.write(f"extern const uint8_t {array_name}[];\n")
        h_file.write(f"extern const uint32_t {array_name}_len;\n\n")
        h_file.write(f"#endif // {array_name.upper()}_H\n")

    # Generate the .c file
    with open(source_filename, "w") as c_file:
        c_file.write(f'#include "{header_filename}"\n\n')
        c_file.write(f"const uint8_t {array_name}[] = {{\n")

        for i, byte in enumerate(data):
            if i % 12 == 0:
                c_file.write("    ")  # Indent nicely
            c_file.write(f"0x{byte:02X}, ")

            if (i + 1) % 12 == 0:
                c_file.write("\n")
        if len(data) % 12 != 0:
            c_file.write("\n")

        c_file.write("};\n\n")
        c_file.write(f"const uint32_t {array_name}_len = {len(data)};\n")

    print(f"Generated {header_filename} and {source_filename}!")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python generate_audio_array.py <input_raw_file> [array_name]")
    else:
        input_file = sys.argv[1]
        array_name = sys.argv[2] if len(sys.argv) > 2 else "audio_data"
        generate_c_array(input_file, array_name)
