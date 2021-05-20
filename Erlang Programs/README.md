## Erlang Program:

### Usage:
Run the following commands:
- `erlc <roll_number>_1.erl `
- `erl -noshell -s <roll_number>_1 main <input_file> <output_file> -s init stop`

### A program to pass an integer token value around all processes in a "Ring-like fashion", and make sure that it does not have a deadlock.

- Create N process, Connect them in ring.
- Pass the token to every process in the ring.
- Once the token reaches the first process the algorithm ends.

### Parallel Merge Sort

- One main process divides the array and sends it to two different processes.
- Those two processes similarly divides the array it has and sends it to two more process.
- This continues till the array size reaches 1.
- Now the locally arrays are sorted at every process and sent to the parent process.
- The parent process merges the array and further sends it to it's parents.
- Finally the main process receives the two sorted arrays and merges them.

