import os

os.system("g++ student.cpp -o A && g++ bot.cpp -o B")
Ascore, Bscore, ACountDown, BCountDown = 0, 0, 0, 0
for i in range(1, 2):
    with open('A.in', 'w') as f:
        f.write(str(i) + "\n")
    with open('88map') as f:
        graph = f.read()
    with open('A.in', 'a') as f:
        f.write(graph + "\n")
    with open('A.in', 'a') as f:
        f.write(str(Ascore) + "\n" + str(Bscore) + "\n" + "A\n")
    os.system("./A < A.in > A.out")
    
# os.system("rm A B *.in *.out")
