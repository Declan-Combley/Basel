# Basel

A very crude stack based programming language that can either be simulated (or compiled eventually).

## Quickstart
```shell
$ ./build.sh
$ ./basel sim example.basel
$ ./basel com example.basel
```

## Running The Code
You can either compile or simulate any program.
```shell
$ ./build.sh
$ ./basel com example.basel && ./out
$ ./basel sim example.basel
```

## Pushing Numbers Onto The Stack
```shell
35 34
1 2 3
```

## Stack Operations
```shell
!      # Pops the value off the top of the stack and prints it
dup    # Clones the value at the top of the stack
over   # Pushes the value left of the stack to the top
+      # Pushes the result of the two popped elements on the stack
-      # Pushes subtracted values of the two popped elements on the stack
*      # Pushes the product of the two popped values at the top stack and pushes it onto the stack
/      # Divides the two popped values at the top stack and pushes it onto the stack
```
