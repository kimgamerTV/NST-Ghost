
def check_braces(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()

    stack = []
    for i, line in enumerate(lines):
        for char in line:
            if char == '{':
                stack.append(i + 1)
            elif char == '}':
                if not stack:
                    print(f"Extra closing brace at line {i + 1}")
                    return
                stack.pop()
    
    if stack:
        print(f"Unclosed opening brace at line {stack[-1]}")
    else:
        print("Braces are balanced.")

check_braces('/home/jop/work/NST/NST/BGA/core/src/engines/rpgm/rpganalyzer.cpp')
