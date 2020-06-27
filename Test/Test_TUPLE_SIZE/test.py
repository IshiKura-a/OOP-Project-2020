import subprocess
import re


N_REC = 30
N_CASE = 2
N_TEST = 6
N_LOG = N_TEST * N_CASE
TEST_SIZE = [10, 25, 50, 75, 100, 200]
_DEBUG = 0


def Generate():
    f = {}

    for i in range(1, N_LOG + 1):
        logName = "test"+str(i)+".log"
        f[i] = open(logName, 'a')

    for i in range(1, N_REC + 1):
        print("Test"+str(i))
        for j in range(0, N_TEST):
            for k in range(1, N_CASE + 1):
                ExeName = "./" + str(k) + ".exe"
                FILE = subprocess.run(args=ExeName, input=str(TEST_SIZE[j]),
                                      capture_output=True, text=True)
                line = FILE.stdout
                print("writing to " + str((k-1)*N_TEST+j+1) + ".log\nSize="+str(TEST_SIZE[j]))
                f[(k-1)*N_TEST+j+1].write(line)

    for i in range(1, N_LOG + 1):
        f[i].close()

    return


def Filter():
    f = {}
    f[0] = open("output.log", 'a')
    string = re.compile('[^0-9\n]*')

    for i in range(1, N_LOG + 1):
        logName = "test"+str(i)+".log"
        f[i] = open(logName, 'r')
        lines = ''.join(f[i].readlines())
        lines = re.sub(string, '', lines)
        f[0].write("Test"+str(i)+'\n')
        f[0].writelines(lines)
        f[0].write("#\n")
        f[i].close()
    f[0].close()
    return


def main():
    if _DEBUG:
        FILE = subprocess.run(args='./1.exe', input=str(200),
                              capture_output=True, text=True)
        print(FILE.stdout)
    else:
        print("Testing...")
        Generate()
        print("Filtering...")
        Filter()

    return


main()
