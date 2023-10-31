#C assembler
C assembler which compiling dummy assembler to machine code.

The assembler works in three steps:
First, it gets an (dummy) assembly file and replace all the macros from it with the specific code of them.
A macro is similar to the idea of function in other languages, when specifing the name of the macro the code inside the macro will run.
After the macros replacement an .am file will be created.
Second, the code will run on the .am file and will create the .ent and the .ext file. 
Third, the assembler will compile the whole assembly code into 0's and 1's in the unique coding of '.' and '/' and will create the .obj file ('.' represent 0 and '/' represent 1).
