import os
import time
import subprocess

print("judging ...")

TIME_OUT = "time out"
INVALID_MOVE = "invalid move"
INVALID_OUTPUT = "invalid output format"
GAME_CLEAR = "game clear"

Ascore, Bscore, ACountDown, BCountDown = 0, 0, 0, 0
msg = ""    
graph = [[0 for _ in range(8)] for _ in range(8)]
with open('88map') as f:
    mygraph = f.read()
for idx, c in enumerate(mygraph[4:]):
    if idx % 2 == 0:
        graph[int(((idx+2)/2-1)//8)][int(((idx+2)/2-1)%8)] = c

def output_graph():
    output = "8 8\n"
    for ls in graph:
        for elm in  ls:
            output += elm + " "
        output = output[0:-1]
        output += "\n"
    return output

def pick_item(who, x, y):
    global Ascore, Bscore, ACountDown, BCountDown
    if who == "A":
        if graph[x][y] == "b":
            print("????")
            ACountDown = 3
        elif graph[x][y] == "m":
            Ascore += 1
        elif graph[x][y] == "n":
            Ascore -= 1
        elif graph[x][y] == "s":
            Ascore *= 2
        elif graph[x][y] == "t":
            if Ascore != 0:
                Ascore //= 2
    else:
        if graph[x][y] == "b":
            BCountDown = 3
        elif graph[x][y] == "m":
            Bscore += 1
        elif graph[x][y] == "n":
            Bscore -= 1
        elif graph[x][y] == "s":
            Bscore *= 2
        elif graph[x][y] == "t":
            if Bscore != 0:
                Bscore //= 2
    graph[x][y] = who


def valid(x, y):
    return (
        x >= 0 and x < 8 and y >=0 and y < 8 and 
        graph[x][y] != 'x' and 
        graph[x][y] != 'A' and graph[x][y] != 'B'
    )

for i in range(1, 1001):
    # Find A, B
    Ax, Ay, Bx, By = 0, 0, 0, 0
    cnt = 0
    for idx, ls in enumerate(graph):
        for idy, elm in enumerate(ls):
            if elm == "A":
                Ax, Ay = idx, idy
            if elm == "B":
                Bx, By = idx, idy
            if elm == "b" or elm == "m" or elm == "n" or elm == "s" or elm == "t":
                cnt += 1
    if cnt == 0:
        msg = GAME_CLEAR
        break;            

    if ACountDown == 0:
        # gen A.in and get ans
        with open('A.in', 'w') as f:
            f.write(str(i) + "\n")
        with open('A.in', 'a') as f:
            f.write(output_graph())
        with open('A.in', 'a') as f:
            f.write(str(Ascore) + "\n" + str(Bscore) + "\n" + "A\n")

        try:
            subprocess.run("./A < A.in > A.out", shell=True, timeout=1)
        except:
            Ascore = -100
            msg = TIME_OUT
            break

        with open('A.out') as f:
            Adir = f.read()[:-1]
        with open('move.log', 'a') as f:
            f.write("A " + Adir + "\n")
        
        # check valid
        if Adir == "UP":
            if valid(Ax-1, Ay):
                graph[Ax][Ay] = "."
                pick_item("A", Ax-1, Ay)
            else:
                Ascore = -100
                msg = INVALID_MOVE
                break
        elif Adir == "DOWN":
            if valid(Ax+1, Ay):
                graph[Ax][Ay] = "."
                pick_item("A", Ax+1, Ay)
            else:
                Ascore = -100
                msg = INVALID_MOVE
                break
        elif Adir == "LEFT":
            if valid(Ax, Ay-1):
                graph[Ax][Ay] = "."
                pick_item("A", Ax, Ay-1)
            else:
                Ascore = -100
                msg = INVALID_MOVE
                break
        elif Adir == "RIGHT":
            if valid(Ax, Ay+1):
                graph[Ax][Ay] = "."
                pick_item("A", Ax, Ay+1)
            else:
                Ascore = -100
                msg = INVALID_MOVE
                break
        else:
            Ascore = -100
            msg = INVALID_OUTPUT
            break
    else:
        ACountDown -= 1

    if BCountDown == 0:
        # gen B.in and get ans
        with open('B.in', 'w') as f:
            f.write(str(i) + "\n")
        with open('B.in', 'a') as f:
            f.write(output_graph())
        with open('B.in', 'a') as f:
            f.write(str(Ascore) + "\n" + str(Bscore) + "\n" + "B\n")

        try:
            subprocess.run("./B < B.in > B.out", shell=True, timeout=1)
        except:
            Bscore = -100
            msg = TIME_OUT
            break

        with open('B.out') as f:
            Bdir = f.read()[:-1]
        with open('move.log', 'a') as f:
            f.write("B " + Bdir + "\n")

        # check valid
        if Bdir == "UP":
            if valid(Bx-1, By):
                graph[Bx][By] = "."
                pick_item("B", Bx-1, By)
            else:
                Bscore = -100
                msg = INVALID_MOVE
                break
        elif Bdir == "DOWN":
            if valid(Bx+1, By):
                graph[Bx][By] = "."
                pick_item("B", Bx+1, By)
            else:
                Bscore = -100
                msg = INVALID_MOVE
                break
        elif Bdir == "LEFT":
            if valid(Bx, By-1):
                graph[Bx][By] = "."
                pick_item("B", Bx, By-1)
            else:
                Bscore = -100
                msg = INVALID_MOVE
                break
        elif Bdir == "RIGHT":
            if valid(Bx, By+1):
                graph[Bx][Ay] = "."
                pick_item("B", Bx, By+1)
            else:
                Bscore = -100
                msg = INVALID_MOVE
                break
        else:
            Bscore = -100
            msg = INVALID_OUTPUT
            break
    else:
        BCountDown -= 1

if Ascore > Bscore:
    with open('move.log', 'a') as f:
        f.write("A Win\n")
elif Ascore < Bscore:
    with open('move.log', 'a') as f:
        f.write("B Win\n")
else:
    with open('move.log', 'a') as f:
        f.write("Tie\n")
with open('move.log', 'a') as f:
    f.write("A Score: " + str(Ascore) + "\n")
    f.write("B Score: " + str(Bscore) + "\n")
    f.write(msg)   
    