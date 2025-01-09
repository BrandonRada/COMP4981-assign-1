## **Running the generate-cmakelists.sh Script**

You will need to create the CMakeLists.txt file:

```bash
./generate-cmakelists.sh
```

## **Running the change-compiler.sh Script**

Tell CMake which compiler you want to use:

```bash
./change-compiler.sh -c <compiler>
```

## **Running the build.sh Script**

To build the program run:

```bash
./build.sh
```

The files.txt file contains:
```<executable> <source files> <header files> <libraries>```

After editing the files.txt
1. run ./generate-cmakelists.sh
2. run ./change-compiler.sh -c <compiler>
3. run ./build.sh

When you need to add/removes files to/from the project you must rerun the 4 steps above. 
