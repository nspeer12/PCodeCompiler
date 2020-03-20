# PCodeCompiler

### How To Run
run in default mode
`bash compile.sh`

## Lost?
`bash compile.sh --help`

## Usage
**note** <filename> is inputs/input0.txt

print the list of lexemes/tokens to the screen
`bash compile.sh -l <filename>`

print the generated assembly code to the screen
`bash compile.sh -a <filename>`

print the virtual machine execution trace
`bash compile.sh -v <filename>`

or use in combination
`bash compile.sh -l -v -a <filename>`
