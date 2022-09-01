import os
from random import Random
import sys



def main():
    someTestsFailed = False
    for i in range(100):
        random = Random()
        expected = random.randint(2, 255)
        size = random.randint(2, 100)
        val1 = random.randint(0, expected)
        val2 = expected - val1
        val1Index = random.randint(0, size - 2)
        val2Index = random.randint(val1Index + 1, size - 1)
        inputString = str(expected) + " " + str(size)
        for i in range(size):
            if i == val1Index: inputString = inputString + " " + str(val1)
            elif i == val2Index: inputString = inputString + " " + str(val2)
            else: inputString = inputString + " " + str(random.randint(2, 255))
        input = open("input.data", "w+")
        input.write(inputString)
        input.close()


        os.system("a.exe ./TwoSum.code ./input.data > ./output")

        output = open("output", "r")
        v1 = int(output.readline())
        v2 = int(output.readline())
        output.close()
        
        someTestsFailed = False

        #if v1 + v2 == expected:
        #    print("Success")
        if v1 + v2 != expected:
            print("Failed on the following test case:")
            print("input string: " + inputString)
            print("expected sum: " + inputString.split(' ')[0])
            print("array size: " + inputString.split(' ')[1] + '\n\n')
            someTestsFailed = True

    if someTestsFailed:
        print("Some Tests failed")
    else: print("All Tests passed")

    input = None

    someTestsFailed = False
    for i in range(100):
        random = Random()
        expected = random.randint(0, 255)
        size = random.randint(1, 100)
        expectedIndex = random.randint(0, size - 1)
        array = [0] * size
        array[expectedIndex] = expected
        inputString = str(expected) + " " + str(size)
        for i in range(size):
            if i == expectedIndex: inputString = inputString + " " + str(expected)
            else:
                randomVal = random.randint(0, 255)
                inputString = inputString + " " + str(randomVal)
                array[i] = randomVal
        input = open("input.data", "w+")
        input.write(inputString)
        input.close()


        os.system("a.exe ./binarySearch.code ./input.data > ./output")

        output = open("output", "r")
        num = output.readline()
        while num:
            num = int(num)
            if num >= size or array[num] != expected:
                print("Failed on the following test case:")
                print("input string: " + inputString)
                print("expected value: " + inputString.split(' ')[0])
                print("array size: " + inputString.split(' ')[1])
                print("output: " + str(num) + '\n\n')
                someTestsFailed = True
                
                sys.stdin.readline()
            num = output.readline()
            num = output.readline()

        output.close()
        

        #if v1 + v2 == expected:
        #    print("Success")




    if someTestsFailed:
        print("Some Tests failed")
    else: print("All Tests passed")
if __name__ == "__main__":
    main()